// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AdvancedSteamSubsystemUtilsPrivatePCH.h"
#include "AdvancedSteamStatsQueryCallbackProxy.h"

#include "AdvancedSteam.h"
#include "OnlineSubsystemSteam.h"
#include "Private/OnlineSubsystemSteamTypes.h"

//////////////////////////////////////////////////////////////////////////
// UAdvancedSteamStatsQueryCallbackProxy

UAdvancedSteamStatsQueryCallbackProxy::UAdvancedSteamStatsQueryCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ReadObject = MakeShareable(new FSteamStatsRead());

#if !WITH_EDITOR
	UAdvancedSteam* AdvancedSteam = Cast<UAdvancedSteam>(UAdvancedSteam::StaticClass()->GetDefaultObject());
	if (AdvancedSteam)
	{
		AdvancedSteamStatsInterface = AdvancedSteam->GetAdvancedSteamStatsInterface();
	}
#endif
}

void UAdvancedSteamStatsQueryCallbackProxy::Activate()
{
#if WITH_EDITOR
	bFailedToEvenSubmit = false;
	OnSuccess.Broadcast(ErrorCode, ReadObject->StatsArray);
#else
	bFailedToEvenSubmit = true;

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub && OnlineSub->GetIdentityInterface().IsValid())
	{
		FUniqueNetIdSteam UserId(*OnlineSub->GetIdentityInterface()->GetUniquePlayerId(0).Get());
		if (AdvancedSteamStatsInterface.IsValid() && UserId.IsValid())
		{
			ReadObject->StatsArray.Empty();
			ReadObject->UserId = FName(*UserId.ToString());
			for (int32 StatIdx = 0; StatIdx < StatsArray.Num(); StatIdx++)
			{
				new (ReadObject->StatsArray) FSteamStats(StatsArray[StatIdx].KeyName, StatsArray[StatIdx].StatsType);
			}

			FSteamStatsReadRef ReadObjectRef = ReadObject.ToSharedRef();

			// Register the completion callback
			StatsReadCompleteDelegate = FOnStatsReadCompleteDelegate::CreateUObject(this, &UAdvancedSteamStatsQueryCallbackProxy::OnStatsRead);
			StatsReadCompleteDelegateHandle = AdvancedSteamStatsInterface->AddOnStatsReadCompleteDelegate_Handle(StatsReadCompleteDelegate);
			if (AdvancedSteamStatsInterface->GetStats(UserId, ReadObjectRef, false))
			{
				bFailedToEvenSubmit = false;
			}
			else
			{
				ErrorCode = EAdvancedSteamErrorCode::STEAM_NOT_INITIALIZED;
				FFrame::KismetExecutionMessage(TEXT("UAdvancedSteamStatsQueryCallbackProxy::TriggerQuery - failed call GetStats"), ELogVerbosity::Warning);
			}
		}
		else
		{
			ErrorCode = EAdvancedSteamErrorCode::ADVANCED_STEAM_MODULE_NOT_INITIALIZED;
			FFrame::KismetExecutionMessage(TEXT("UAdvancedSteamStatsQueryCallbackProxy::TriggerQuery - AdvancedSteamStats Invalid or Cannot map local player to unique net ID"), ELogVerbosity::Warning);
		}
	}
	else
	{
		ErrorCode = EAdvancedSteamErrorCode::STEAM_NOT_INITIALIZED;
		FFrame::KismetExecutionMessage(TEXT("UAdvancedSteamStatsQueryCallbackProxy::TriggerQuery - Invalid player state"), ELogVerbosity::Warning);
	}

	if (bFailedToEvenSubmit)
	{
		OnStatsRead(false, ErrorCode);
	}
#endif
}

void UAdvancedSteamStatsQueryCallbackProxy::OnStatsRead(bool bWasSuccessful, const EAdvancedSteamErrorCode& InErrorCode)
{
	RemoveDelegate();


	if (bWasSuccessful)
	{
		OnSuccess.Broadcast(ErrorCode, ReadObject->StatsArray);
	}
	else
	{
		ErrorCode = InErrorCode;
		OnFailure.Broadcast(ErrorCode, ReadObject->StatsArray);
	}
}

void UAdvancedSteamStatsQueryCallbackProxy::RemoveDelegate()
{
#if !WITH_EDITOR
	if (AdvancedSteamStatsInterface.IsValid() && !bFailedToEvenSubmit)
	{
		AdvancedSteamStatsInterface->ClearOnStatsReadCompleteDelegate_Handle(StatsReadCompleteDelegateHandle);
	}
#endif
}

void UAdvancedSteamStatsQueryCallbackProxy::BeginDestroy()
{
	RemoveDelegate();

	Super::BeginDestroy();
}

UAdvancedSteamStatsQueryCallbackProxy* UAdvancedSteamStatsQueryCallbackProxy::CreateProxyObjectForIntQuery(TArray<FSteamStats> InStatsArray)
{
	UAdvancedSteamStatsQueryCallbackProxy* Proxy = NewObject<UAdvancedSteamStatsQueryCallbackProxy>();
	Proxy->SetFlags(RF_StrongRefOnFrame);
	Proxy->StatsArray = InStatsArray;
	return Proxy;
}
