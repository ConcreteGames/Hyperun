// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AdvancedSteamSubsystemUtilsPrivatePCH.h"
#include "AdvancedSteamAchievementUnlockCallbackProxy.h"

#include "AdvancedSteam.h"
#include "OnlineSubsystemSteam.h"

//////////////////////////////////////////////////////////////////////////
// UAdvancedSteamAchievementUnlockCallbackProxy

UAdvancedSteamAchievementUnlockCallbackProxy::UAdvancedSteamAchievementUnlockCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if !WITH_EDITOR
	UAdvancedSteam* AdvancedSteam = Cast<UAdvancedSteam>(UAdvancedSteam::StaticClass()->GetDefaultObject());
	if (AdvancedSteam)
	{
		AdvancedSteamAchievementsInterface = AdvancedSteam->GetAdvancedSteamAchievementsInterface();
	}
#endif
}

void UAdvancedSteamAchievementUnlockCallbackProxy::Activate()
{
#if WITH_EDITOR
	bFailedToEvenSubmit = false;
	OnSuccess.Broadcast(ErrorCode);
#else
	bFailedToEvenSubmit = true;

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub && OnlineSub->GetIdentityInterface().IsValid())
	{
		FUniqueNetIdSteam SteamUserId(*OnlineSub->GetIdentityInterface()->GetUniquePlayerId(0).Get());
		UserId = SteamUserId;

		if (AdvancedSteamAchievementsInterface.IsValid() && UserId.IsValid())
		{
			// Register the completion callback
			AchievementsUpdateCompleteDelegate = FOnAchievementsUpdateCompleteDelegate::CreateUObject(this, &UAdvancedSteamAchievementUnlockCallbackProxy::OnAchievementsUpdate);
			AchievementsUpdateCompleteDelegateHandle = AdvancedSteamAchievementsInterface->AddOnAchievementsUpdateCompleteDelegate_Handle(AchievementsUpdateCompleteDelegate);
			if (AdvancedSteamAchievementsInterface->SetAchievement(UserId, AchievementId))
			{
				bFailedToEvenSubmit = false;
			}
			else
			{
				ErrorCode = EAdvancedSteamErrorCode::ADVANCED_STEAM_MODULE_NOT_INITIALIZED;
				FFrame::KismetExecutionMessage(TEXT("UAdvancedSteamAchievementUnlockCallbackProxy::TriggerQuery - failed call GetAchievements"), ELogVerbosity::Warning);
			}
		}
		else
		{
			ErrorCode = EAdvancedSteamErrorCode::INVALID_PLAYER_ID;
			FFrame::KismetExecutionMessage(TEXT("UAdvancedSteamAchievementUnlockCallbackProxy::TriggerQuery - AdvancedSteamAchievements Invalid or Cannot map local player to unique net ID"), ELogVerbosity::Warning);
		}
	}
	else
	{
		ErrorCode = EAdvancedSteamErrorCode::INVALID_PLAYER_STATE;
		FFrame::KismetExecutionMessage(TEXT("UAdvancedSteamAchievementUnlockCallbackProxy::TriggerQuery - Invalid player state"), ELogVerbosity::Warning);
	}

	if (bFailedToEvenSubmit)
	{
		OnAchievementsUpdate(false, ErrorCode);
	}
#endif
}

void UAdvancedSteamAchievementUnlockCallbackProxy::OnAchievementsUpdate(bool bWasSuccessful, const EAdvancedSteamErrorCode& InErrorCode)
{
	RemoveDelegate();

	if (bWasSuccessful)
	{
		OnSuccess.Broadcast(ErrorCode);
	}
	else
	{
		OnFailure.Broadcast(ErrorCode);
	}
}

void UAdvancedSteamAchievementUnlockCallbackProxy::RemoveDelegate()
{
#if !WITH_EDITOR
	if (AdvancedSteamAchievementsInterface.IsValid() && !bFailedToEvenSubmit)
	{
		AdvancedSteamAchievementsInterface->ClearOnAchievementsUpdateCompleteDelegate_Handle(AchievementsUpdateCompleteDelegateHandle);
	}
#endif
}

void UAdvancedSteamAchievementUnlockCallbackProxy::BeginDestroy()
{
	RemoveDelegate();

	Super::BeginDestroy();
}

UAdvancedSteamAchievementUnlockCallbackProxy* UAdvancedSteamAchievementUnlockCallbackProxy::CreateProxyObjectForAchievementsUnlock(FName AchievementId)
{
	UAdvancedSteamAchievementUnlockCallbackProxy* Proxy = NewObject<UAdvancedSteamAchievementUnlockCallbackProxy>();
	Proxy->SetFlags(RF_StrongRefOnFrame);
	Proxy->AchievementId = AchievementId;
	return Proxy;
}
