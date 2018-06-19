// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AdvancedSteamSubsystemUtilsPrivatePCH.h"
#include "AdvancedSteamAchievementQueryCallbackProxy.h"

#include "AdvancedSteam.h"
#include "OnlineSubsystemSteam.h"

//////////////////////////////////////////////////////////////////////////
// UAdvancedSteamAchievementQueryCallbackProxy

UAdvancedSteamAchievementQueryCallbackProxy::UAdvancedSteamAchievementQueryCallbackProxy(const FObjectInitializer& ObjectInitializer)
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

void UAdvancedSteamAchievementQueryCallbackProxy::Activate()
{
#if WITH_EDITOR
	bFailedToEvenSubmit = false;
	OnSuccess.Broadcast(ErrorCode, TArray<FSteamAchievements>());
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
			AchievementsReadCompleteDelegate = FOnAchievementsReadCompleteDelegate::CreateUObject(this, &UAdvancedSteamAchievementQueryCallbackProxy::OnAchievementsRead);
			AchievementsReadCompleteDelegateHandle = AdvancedSteamAchievementsInterface->AddOnAchievementsReadCompleteDelegate_Handle(AchievementsReadCompleteDelegate);
			if (AdvancedSteamAchievementsInterface->QueryAchievements(UserId))
			{
				bFailedToEvenSubmit = false;
			}
			else
			{
				ErrorCode = EAdvancedSteamErrorCode::ADVANCED_STEAM_MODULE_NOT_INITIALIZED;
				FFrame::KismetExecutionMessage(TEXT("UAdvancedSteamAchievementQueryCallbackProxy::TriggerQuery - failed call GetAchievements"), ELogVerbosity::Warning);
			}
		}
		else
		{
			ErrorCode = EAdvancedSteamErrorCode::INVALID_PLAYER_ID;
			FFrame::KismetExecutionMessage(TEXT("UAdvancedSteamAchievementQueryCallbackProxy::TriggerQuery - AdvancedSteamAchievements Invalid or Cannot map local player to unique net ID"), ELogVerbosity::Warning);
		}
	}
	else
	{
		ErrorCode = EAdvancedSteamErrorCode::INVALID_PLAYER_STATE;
		FFrame::KismetExecutionMessage(TEXT("UAdvancedSteamAchievementQueryCallbackProxy::TriggerQuery - Invalid player state"), ELogVerbosity::Warning);
	}

	if (bFailedToEvenSubmit)
	{
		OnAchievementsRead(false, ErrorCode);
	}
#endif
}

void UAdvancedSteamAchievementQueryCallbackProxy::OnAchievementsRead(bool bWasSuccessful, const EAdvancedSteamErrorCode& InErrorCode)
{
	RemoveDelegate();

	if (bWasSuccessful)
	{
#if WITH_EDITOR
		OnSuccess.Broadcast(ErrorCode, TArray<FSteamAchievements>());
#else
		if (AdvancedSteamAchievementsInterface.IsValid())
		{
			TArray<FSteamAchievements> OutAchievementsArray;
			AdvancedSteamAchievementsInterface->GetAchievements(UserId, OutAchievementsArray);
			OnSuccess.Broadcast(ErrorCode, OutAchievementsArray);
		}
		else
		{
			ErrorCode = EAdvancedSteamErrorCode::ADVANCED_STEAM_MODULE_NOT_INITIALIZED;
			OnFailure.Broadcast(ErrorCode, TArray<FSteamAchievements>());
		}
#endif
	}
	else
	{
		ErrorCode = InErrorCode;
		OnFailure.Broadcast(ErrorCode, TArray<FSteamAchievements>());
	}
}

void UAdvancedSteamAchievementQueryCallbackProxy::RemoveDelegate()
{
#if !WITH_EDITOR
	if (AdvancedSteamAchievementsInterface.IsValid() && !bFailedToEvenSubmit)
	{
		AdvancedSteamAchievementsInterface->ClearOnAchievementsReadCompleteDelegate_Handle(AchievementsReadCompleteDelegateHandle);
	}
#endif
}

void UAdvancedSteamAchievementQueryCallbackProxy::BeginDestroy()
{
	RemoveDelegate();

	Super::BeginDestroy();
}

UAdvancedSteamAchievementQueryCallbackProxy* UAdvancedSteamAchievementQueryCallbackProxy::CreateProxyObjectForAchievementsQuery()
{
	UAdvancedSteamAchievementQueryCallbackProxy* Proxy = NewObject<UAdvancedSteamAchievementQueryCallbackProxy>();
	Proxy->SetFlags(RF_StrongRefOnFrame);
	return Proxy;
}
