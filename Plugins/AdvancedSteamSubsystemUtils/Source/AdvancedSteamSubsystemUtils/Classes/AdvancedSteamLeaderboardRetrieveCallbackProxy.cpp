// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AdvancedSteamSubsystemUtilsPrivatePCH.h"
#include "AdvancedSteamLeaderboardRetrieveCallbackProxy.h"

#include "AdvancedSteam.h"
#include "OnlineSubsystemSteam.h"
#include "Private/OnlineSubsystemSteamTypes.h"

//////////////////////////////////////////////////////////////////////////
// ULeaderboardRetrieveCallbackProxy

UAdvancedSteamLeaderboardRetrieveCallbackProxy::UAdvancedSteamLeaderboardRetrieveCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if !WITH_EDITOR
	UAdvancedSteam* AdvancedSteam = Cast<UAdvancedSteam>(UAdvancedSteam::StaticClass()->GetDefaultObject());
	if (AdvancedSteam)
	{
		AdvancedSteamLeaderboardInterface = AdvancedSteam->GetAdvancedSteamLeaderboardInterface();
		AdvancedSteamStatsInterface = AdvancedSteam->GetAdvancedSteamStatsInterface();
	}
#endif
}

void UAdvancedSteamLeaderboardRetrieveCallbackProxy::Activate()
{
#if WITH_EDITOR
	OnSuccess.Broadcast(ErrorCode, TArray<FSteamLeaderboard>());
#else
	bFailedToEvenSubmit = true;
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub && OnlineSub->GetIdentityInterface().IsValid())
	{
		TSharedPtr<const FUniqueNetId> UserId = OnlineSub->GetIdentityInterface()->GetUniquePlayerId(0);

		if (AdvancedSteamLeaderboardInterface.IsValid() && UserId.IsValid())
		{
			EntriesReadCompleteDelegate = FOnEntriesReadCompleteDelegate::CreateUObject(this, &UAdvancedSteamLeaderboardRetrieveCallbackProxy::OnEntriesRead);
			EntriesReadCompleteDelegateHandle = AdvancedSteamLeaderboardInterface->AddOnEntriesReadCompleteDelegate_Handle(EntriesReadCompleteDelegate);

			TArray< TSharedRef<const FUniqueNetId> > ListOfIDs;
			ListOfIDs.Add(UserId.ToSharedRef());

			if (!ReadObject.IsValid())
			{
				ReadObject = MakeShareable(new FSteamLeaderboardRead());
			}

			ReadObject->LeaderboardName = LeaboardName;
			ReadObject->SortedColumn = StatName;
			new (ReadObject->ColumnMetadata) FLeaderboardMetadata(StatName, ELeaderboardSort::Descending, ELeaderboardFormat::Number);

			FSteamLeaderboardReadRef ReadObjectRef = ReadObject.ToSharedRef();

			if (AdvancedSteamLeaderboardInterface->DownloadLeaderboardEntries(ListOfIDs, ReadObjectRef, LeaderboardRequest, RangeStart, RangeEnd, DetailCount))
			{
				bFailedToEvenSubmit = false;
				FFrame::KismetExecutionMessage(TEXT("ULeaderboardRetrieveCallbackProxy::Activate - called DownloadLeaderboardEntries"), ELogVerbosity::Warning);
			}
			else
			{
				ErrorCode = EAdvancedSteamErrorCode::STEAM_NOT_INITIALIZED;
				FFrame::KismetExecutionMessage(TEXT("ULeaderboardRetrieveCallbackProxy::Activate - failed call DownloadLeaderboardEntries"), ELogVerbosity::Warning);
			}
		}
		else
		{
			ErrorCode = EAdvancedSteamErrorCode::INVALID_PLAYER_ID;
			FFrame::KismetExecutionMessage(TEXT("ULeaderboardRetrieveCallbackProxy::Activate - AdvancedSteamLeaderboard Invalid or Cannot map local player to unique net ID"), ELogVerbosity::Warning);
		}
	}
	else
	{
		ErrorCode = EAdvancedSteamErrorCode::INVALID_PLAYER_STATE;
		FFrame::KismetExecutionMessage(TEXT("ULeaderboardRetrieveCallbackProxy::Activate - Invalid player state"), ELogVerbosity::Warning);
	}

	if (bFailedToEvenSubmit)
	{
		OnEntriesRead(false, ErrorCode);
	}
#endif
}

void UAdvancedSteamLeaderboardRetrieveCallbackProxy::OnEntriesRead(bool InWasSuccessful, const EAdvancedSteamErrorCode& InErrorCode)
{
	RemoveDelegate();

	if (InWasSuccessful)
	{
#if WITH_EDITOR
		OnSuccess.Broadcast(ErrorCode, TArray<FSteamLeaderboard>());
		ReadObject = nullptr;
#else
		// request get stats
		FScopeLock ScopeLock(&StatsLock);
		if (!ReadObject.IsValid())
		{
			ReadObject = MakeShareable(new FSteamLeaderboardRead());
		}
		if (AdvancedSteamStatsInterface.IsValid() && ReadObject->Rows.Num() > 0)
		{
			RequestedStatsCount = ReadObject->Rows.Num();
			// Register the completion callback
			StatsReadRefCompleteDelegate = FOnStatsReadRefCompleteDelegate::CreateUObject(this, &UAdvancedSteamLeaderboardRetrieveCallbackProxy::OnStatsRead);
			StatsReadRefCompleteDelegateHandle = AdvancedSteamStatsInterface->AddOnStatsReadRefCompleteDelegate_Handle(StatsReadRefCompleteDelegate);

			bool Success = true;
			for (int32 EntryIdx = 0; EntryIdx < ReadObject->Rows.Num(); EntryIdx++)
			{
				FSteamLeaderboard& Leaderboard = ReadObject->Rows[EntryIdx];
				FUniqueNetIdSteam UserId(Leaderboard.PlayerId.ToString());

				TSharedPtr<class FSteamStatsRead, ESPMode::ThreadSafe> ReadStatsObject = MakeShareable(new FSteamStatsRead());
				ReadStatsObject->UserId = Leaderboard.PlayerId;

				for (int32 StatIdx = 0; StatIdx < StatsArray.Num(); StatIdx++)
				{
					new (ReadStatsObject->StatsArray) FSteamStats(StatsArray[StatIdx].KeyName, StatsArray[StatIdx].StatsType);
				}

				FSteamStatsReadRef ReadObjectStatsRef = ReadStatsObject.ToSharedRef();

				// get steam stats
				if (AdvancedSteamStatsInterface->GetStats(UserId, ReadObjectStatsRef, true))
				{
					FFrame::KismetExecutionMessage(*FString::Printf(TEXT("FOnlineAsyncTaskSteamGetLeaderboardEntries::TriggerQuery Requested stats : %s"), *UserId.ToString()), ELogVerbosity::Display);
				}
				else
				{
					Success = false;
					--RequestedStatsCount;
					ErrorCode = EAdvancedSteamErrorCode::STEAM_NOT_INITIALIZED;
					FFrame::KismetExecutionMessage(TEXT("FOnlineAsyncTaskSteamGetLeaderboardEntries::TriggerQuery - failed call GetStats"), ELogVerbosity::Warning);

					// return without stats
					OnSuccess.Broadcast(ErrorCode, ReadObject->Rows);
					ReadObject = nullptr;
					break;
				}
			}
			if (!Success)
			{
				RemoveStatsDelegate();
				OnFailure.Broadcast(ErrorCode, TArray<FSteamLeaderboard>());
			}
		}
		else
		{
			// return without stats
			OnSuccess.Broadcast(ErrorCode, ReadObject->Rows);
			ReadObject = nullptr;
			FFrame::KismetExecutionMessage(TEXT("FOnlineAsyncTaskSteamGetLeaderboardEntries::TriggerQuery - AdvancedSteamStats Invalid or Cannot map local player to unique net ID"), ELogVerbosity::Warning);
		}
#endif
	}
	else
	{
		OnFailure.Broadcast(ErrorCode, TArray<FSteamLeaderboard>());
		ReadObject = nullptr;
	}

}

void UAdvancedSteamLeaderboardRetrieveCallbackProxy::RemoveDelegate()
{
#if !WITH_EDITOR
	if (AdvancedSteamLeaderboardInterface.IsValid() && !bFailedToEvenSubmit)
	{
		AdvancedSteamLeaderboardInterface->ClearOnEntriesReadCompleteDelegate_Handle(EntriesReadCompleteDelegateHandle);
	}
#endif
}

void UAdvancedSteamLeaderboardRetrieveCallbackProxy::OnStatsRead(bool InWasSuccessful, const EAdvancedSteamErrorCode& InErrorCode, const FSteamStatsReadPtr SteamStatsReadPtr)
{
	FScopeLock ScopeLock(&StatsLock);
	--RequestedStatsCount;

	if (InWasSuccessful)		
	{
		// find players
		FSteamLeaderboard* SteamLeaderboardPtr = ReadObject->FindPlayerRecord(SteamStatsReadPtr->UserId);
		if (SteamLeaderboardPtr && SteamStatsReadPtr.IsValid())
		{
			// add stats
			SteamLeaderboardPtr->Columns = SteamStatsReadPtr->StatsArray;
		}

		if (SteamStatsReadPtr.IsValid())
		{
			FFrame::KismetExecutionMessage(*FString::Printf(TEXT("RequestedStatsCount %d %s"), RequestedStatsCount, *SteamStatsReadPtr->UserId.ToString()), ELogVerbosity::Display);
		}
		
		if (0 == RequestedStatsCount) 
		{
			RemoveStatsDelegate();
			if (SteamStatsReadPtr.IsValid())
			{
				FFrame::KismetExecutionMessage(*FString::Printf(TEXT("FOnlineAsyncTaskSteamGetLeaderboardEntries::OnStatsRead Success : %s"), *SteamStatsReadPtr->UserId.ToString()), ELogVerbosity::Display);
			}
			OnSuccess.Broadcast(ErrorCode, ReadObject->Rows);
		}
	}
	else
	{
		ErrorCode = InErrorCode;
		OnFailure.Broadcast(ErrorCode, ReadObject->Rows);
	}
}

void UAdvancedSteamLeaderboardRetrieveCallbackProxy::RemoveStatsDelegate()
{
#if !WITH_EDITOR
	if (AdvancedSteamStatsInterface.IsValid())
	{
		AdvancedSteamStatsInterface->ClearOnStatsReadRefCompleteDelegate_Handle(StatsReadRefCompleteDelegateHandle);
	}
#endif
}

void UAdvancedSteamLeaderboardRetrieveCallbackProxy::BeginDestroy()
{
	ReadObject = nullptr;
	RemoveDelegate();
	RemoveStatsDelegate();

	Super::BeginDestroy();
}

UAdvancedSteamLeaderboardRetrieveCallbackProxy* UAdvancedSteamLeaderboardRetrieveCallbackProxy::CreateProxyObjectForRetrieve(FName LeaboardName, FName StatName, TArray<FSteamStats> StatsArray, ELeaderboardRequest LeaderboardRequest, int32 RangeStart, int32 RangeEnd, int32 DetailCount)
{
	UAdvancedSteamLeaderboardRetrieveCallbackProxy* Proxy = NewObject<UAdvancedSteamLeaderboardRetrieveCallbackProxy>();
	Proxy->SetFlags(RF_StrongRefOnFrame);
	Proxy->LeaboardName = LeaboardName;
	Proxy->StatName = StatName;
	Proxy->StatsArray = StatsArray;
	Proxy->LeaderboardRequest = LeaderboardRequest;
	Proxy->RangeStart = RangeStart;
	Proxy->RangeEnd = RangeEnd;
	Proxy->DetailCount = DetailCount;
	return Proxy;
}