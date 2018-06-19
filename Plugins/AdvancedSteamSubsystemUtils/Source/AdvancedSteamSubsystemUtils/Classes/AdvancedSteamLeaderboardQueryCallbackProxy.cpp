// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AdvancedSteamSubsystemUtilsPrivatePCH.h"
#include "AdvancedSteamLeaderboardQueryCallbackProxy.h"

#include "OnlineSubsystemSteam.h"
#include "Private/OnlineSubsystemSteamTypes.h"

//////////////////////////////////////////////////////////////////////////
// ULeaderboardQueryCallbackProxy

UAdvancedSteamLeaderboardQueryCallbackProxy::UAdvancedSteamLeaderboardQueryCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAdvancedSteamLeaderboardQueryCallbackProxy::Activate()
{
#if WITH_EDITOR
	OnSuccess.Broadcast(SavedValue);
#else
	bFailedToEvenSubmit = true;

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub && OnlineSub->GetIdentityInterface().IsValid())
	{
		TSharedPtr<const FUniqueNetId> UserId = OnlineSub->GetIdentityInterface()->GetUniquePlayerId(0);
		if (UserId.IsValid())
		{
			IOnlineLeaderboardsPtr Leaderboards = OnlineSub->GetLeaderboardsInterface();
			if (Leaderboards.IsValid())
			{
				bFailedToEvenSubmit = false;

				StatName = StatName;
				ReadObject = MakeShareable(new FOnlineLeaderboardRead());
				ReadObject->LeaderboardName = LeaboardName;
				ReadObject->SortedColumn = StatName;
				new (ReadObject->ColumnMetadata) FColumnMetaData(StatName, EOnlineKeyValuePairDataType::Int32);

				// Register the completion callback
				LeaderboardReadCompleteDelegate       = FOnLeaderboardReadCompleteDelegate::CreateUObject(this, &UAdvancedSteamLeaderboardQueryCallbackProxy::OnStatsRead);
				LeaderboardReadCompleteDelegateHandle = Leaderboards->AddOnLeaderboardReadCompleteDelegate_Handle(LeaderboardReadCompleteDelegate);

				TArray< TSharedRef<const FUniqueNetId> > ListOfIDs;
				ListOfIDs.Add(UserId.ToSharedRef());

				FOnlineLeaderboardReadRef ReadObjectRef = ReadObject.ToSharedRef();
				Leaderboards->ReadLeaderboards(ListOfIDs, ReadObjectRef);
			}
			else
			{
				FFrame::KismetExecutionMessage(TEXT("ULeaderboardQueryCallbackProxy::TriggerQuery - Leaderboards not supported by Online Subsystem"), ELogVerbosity::Warning);
			}
		}
		else
		{
			FFrame::KismetExecutionMessage(TEXT("ULeaderboardQueryCallbackProxy::TriggerQuery - Cannot map local player to unique net ID"), ELogVerbosity::Warning);
		}
	}
	else
	{
		FFrame::KismetExecutionMessage(TEXT("ULeaderboardQueryCallbackProxy::TriggerQuery - Invalid player state"), ELogVerbosity::Warning);
	}

	if (bFailedToEvenSubmit)
	{
		OnStatsRead(false);
	}
#endif
}

void UAdvancedSteamLeaderboardQueryCallbackProxy::OnStatsRead(bool bWasSuccessful)
{
	RemoveDelegate();

	bool bFoundValue = false;
	int32 Value = 0;

	if (bWasSuccessful)
	{
		if (ReadObject.IsValid())
		{
			for (int Idx = 0; Idx < ReadObject->Rows.Num(); ++Idx)
			{
				FVariantData* Variant = ReadObject->Rows[Idx].Columns.Find(StatName);

				if (Variant != nullptr)
				{
					bFoundValue = true;
					Variant->GetValue(Value);
				}
			}
		}
	}

	if (bFoundValue)
	{
		SavedValue = Value;
		OnSuccess.Broadcast(SavedValue);
	}
	else
	{
		SavedValue = 0;
		OnFailure.Broadcast(0);
	}

	ReadObject = nullptr;
}

void UAdvancedSteamLeaderboardQueryCallbackProxy::RemoveDelegate()
{
	if (!bFailedToEvenSubmit)
	{
		if (IOnlineSubsystem* OnlineSub = IOnlineSubsystem::IsLoaded() ? IOnlineSubsystem::Get() : nullptr)
		{
			IOnlineLeaderboardsPtr Leaderboards = OnlineSub->GetLeaderboardsInterface();
			if (Leaderboards.IsValid())
			{
				Leaderboards->ClearOnLeaderboardReadCompleteDelegate_Handle(LeaderboardReadCompleteDelegateHandle);
			}
		}
	}
}

void UAdvancedSteamLeaderboardQueryCallbackProxy::BeginDestroy()
{
	ReadObject = nullptr;
	RemoveDelegate();

	Super::BeginDestroy();
}

UAdvancedSteamLeaderboardQueryCallbackProxy* UAdvancedSteamLeaderboardQueryCallbackProxy::CreateProxyObjectForIntQuery(FName LeaboardName, FName StatName)
{
	UAdvancedSteamLeaderboardQueryCallbackProxy* Proxy = NewObject<UAdvancedSteamLeaderboardQueryCallbackProxy>();
	Proxy->SetFlags(RF_StrongRefOnFrame);
	Proxy->LeaboardName = LeaboardName;
	Proxy->StatName = StatName;
	return Proxy;
}
