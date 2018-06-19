// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/**
*	Async task to retrieve leaderboard entries for a single user from the Steam backend
*/
#include "AdvancedSteamPrivatePCH.h"
#include "OnlineAsyncTaskSteamGetLeaderboardEntries.h"

#include "AdvancedSteamLeaderboard.h"
#include "OnlineSubsystemSteam.h"
#include "Private/OnlineAsyncTaskManagerSteam.h"
#include "Private/OnlineLeaderboardInterfaceSteam.h"
#include "steam/isteamuserstats.h"

/** Helper function to convert enums */
inline ELeaderboardDataRequest ToSteamLeaderboardDataRequest(ELeaderboardRequest InLeaderboardRequest)
{
	switch (InLeaderboardRequest)
	{
	case ELeaderboardRequest::ELeaderboardRequestGlobal:
		return ELeaderboardDataRequest::k_ELeaderboardDataRequestGlobal;
	case ELeaderboardRequest::ELeaderboardRequestGlobalAroundUser:
		return ELeaderboardDataRequest::k_ELeaderboardDataRequestGlobalAroundUser;
	case ELeaderboardRequest::ELeaderboardRequestFriends:
		return ELeaderboardDataRequest::k_ELeaderboardDataRequestFriends;
	case ELeaderboardRequest::ELeaderboardRequestUsers:
	default:
		return ELeaderboardDataRequest::k_ELeaderboardDataRequestGlobal;
	}
}

FOnlineAsyncTaskSteamGetLeaderboardEntries::FOnlineAsyncTaskSteamGetLeaderboardEntries(
	FOnlineSubsystemSteam* InSteamSubsystem,
	const TArray< TSharedRef<const FUniqueNetId> >& InPlayers,
	const FSteamLeaderboardReadRef& InReadObject,
	FAdvancedSteamLeaderboard* InAdvancedSteamLeaderboardPtr, 
	ELeaderboardRequest InLeaderboardDataRequest, 
	int32 InRangeStart, 
	int32 InRangeEnd, 
	int32 InDetailCount,
	bool InShouldTriggerDelegates)
	:
	FOnlineAsyncTaskSteam(InSteamSubsystem, k_uAPICallInvalid),
	bInit(false),
	Players(InPlayers),
	ReadObject(InReadObject),
	AdvancedSteamLeaderboardPtr(InAdvancedSteamLeaderboardPtr),
	LeaderboardRequest(InLeaderboardDataRequest),
	RangeStart(InRangeStart), 
	RangeEnd(InRangeEnd),
	DetailCount(InDetailCount),
	ShouldTriggerDelegates(InShouldTriggerDelegates)
{
}

/**
*	Get a human readable description of task
*/
FString FOnlineAsyncTaskSteamGetLeaderboardEntries::ToString() const 
{
	return FString::Printf(TEXT("FOnlineAsyncTaskSteamGetLeaderboardEntries bWasSuccessful: %d"), bWasSuccessful);
}

/**
* Give the async task time to do its work
* Can only be called on the async task manager thread
*/
void FOnlineAsyncTaskSteamGetLeaderboardEntries::Tick()
{
	ISteamUtils* SteamUtilsPtr = SteamUtils();
	check(SteamUtilsPtr);

	if (!bInit)
	{
		// Poll for leaderboard handle
		SteamLeaderboard_t LeaderboardHandle = -1;
		{

#if !WITH_EDITOR
			FLeaderboardMetadata* Leaderboard = AdvancedSteamLeaderboardPtr->GetLeaderboardMetadata(ReadObject->LeaderboardName);
			if (Leaderboard)
			{
				LeaderboardHandle = Leaderboard->LeaderboardHandle;
			}
#endif
		}

		if (LeaderboardHandle != -1)
		{
			ISteamUserStats* SteamUserStatsPtr = SteamUserStats();
			CallbackHandle = SteamUserStatsPtr->DownloadLeaderboardEntries(LeaderboardHandle, ToSteamLeaderboardDataRequest(LeaderboardRequest), RangeStart, RangeEnd);
			// Max leaderboard entries is 100
			/*
			int32 NumUsers = FPlatformMath::Min(Players.Num(), 100);
			CSteamID* IdArray = new CSteamID[NumUsers];
			for (int32 UserIdx = 0; UserIdx<NumUsers; UserIdx++)
			{
				IdArray[UserIdx] = *(uint64*)Players[UserIdx]->GetBytes();
			}
			delete[] IdArray;
			*/

			bInit = true;
		}
	}

	if (CallbackHandle != k_uAPICallInvalid)
	{
		bool bFailedCall = false;

		// Poll for completion status
		bIsComplete = SteamUtilsPtr->IsAPICallCompleted(CallbackHandle, &bFailedCall) ? true : false;
		if (bIsComplete)
		{
			bool bFailedResult;
			// Retrieve the callback data from the request
			bool bSuccessCallResult = SteamUtilsPtr->GetAPICallResult(CallbackHandle, &CallbackResults, sizeof(CallbackResults), CallbackResults.k_iCallback, &bFailedResult);
			bWasSuccessful = (bSuccessCallResult ? true : false) &&
				(!bFailedCall ? true : false) &&
				(!bFailedResult ? true : false) &&
				((CallbackResults.m_hSteamLeaderboard != -1) ? true : false);
		}
	}
	else if (bInit)
	{
		// Invalid API call
		bIsComplete = true;
		bWasSuccessful = false;
		ErrorCode = EAdvancedSteamErrorCode::UNKNOWN;
	}
}

/**
* Give the async task a chance to marshal its data back to the game thread
* Can only be called on the game thread by the async task manager
*/
void FOnlineAsyncTaskSteamGetLeaderboardEntries::Finalize()
{
	FOnlineAsyncTaskSteam::Finalize();
	
	ISteamUserStats* SteamUserStatsPtr = SteamUserStats();
	for (int32 EntryIdx = 0; EntryIdx < CallbackResults.m_cEntryCount; EntryIdx++)
	{
		LeaderboardEntry_t LeaderboardEntry;
		int32 *Details = nullptr;
		if (DetailCount > 0)
		{
			Details = new int32[DetailCount];
		}

		if (SteamUserStatsPtr->GetDownloadedLeaderboardEntry(CallbackResults.m_hSteamLeaderboardEntries, EntryIdx, &LeaderboardEntry, Details, DetailCount))
		{
			FUniqueNetIdSteam CurrentUser(LeaderboardEntry.m_steamIDUser);
			FSteamLeaderboard* UserRow = ReadObject->FindPlayerRecord(FName(*CurrentUser.ToString()));
			if (UserRow == nullptr)
			{
				TArray<int32> DetailsArray;
				if (DetailCount > 0 && Details)
				{
					DetailsArray.Insert(Details, DetailCount, 0);
				}
				const FName NickName(UTF8_TO_TCHAR(SteamFriends()->GetFriendPersonaName(LeaderboardEntry.m_steamIDUser)));
				TSharedRef<const FUniqueNetIdSteam> UserId = MakeShareable(new FUniqueNetIdSteam(LeaderboardEntry.m_steamIDUser));
				int32 IconImage = SteamFriends()->GetMediumFriendAvatar(LeaderboardEntry.m_steamIDUser);
				UserRow = new (ReadObject->Rows) FSteamLeaderboard(NickName, FName(*UserId->ToString()), LeaderboardEntry.m_nScore, LeaderboardEntry.m_nGlobalRank, DetailsArray, IconImage);
				if (LeaderboardEntry.m_hUGC == k_UGCHandleInvalid)
				{
					UserRow->HandleUGC = TEXT("Invalid");
				}
				else
				{
					// need to check on mac
					UserRow->HandleUGC = FString::Printf(TEXT("%I64d"), (int64)LeaderboardEntry.m_hUGC);
				}
			}
		}

		if (Details)
		{
			delete Details;
		}
	}
}

void FOnlineAsyncTaskSteamGetLeaderboardEntries::TriggerDelegates()
{
	FOnlineAsyncTaskSteam::TriggerDelegates();
#if !WITH_EDITOR
	if (ShouldTriggerDelegates)
	{
		AdvancedSteamLeaderboardPtr->TriggerOnEntriesReadCompleteDelegates(bWasSuccessful, ErrorCode);
	}
#endif
}
