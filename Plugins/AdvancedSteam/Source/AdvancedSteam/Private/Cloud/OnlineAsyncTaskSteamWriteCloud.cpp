// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/**
*	Async task to update leaderboard for a single user from the Steam backend
*/
#include "AdvancedSteamPrivatePCH.h"
#include "OnlineAsyncTaskSteamWriteCloud.h"

#include "AdvancedSteamCloud.h"
#include "OnlineSubsystemSteam.h"
#include "Private/OnlineAsyncTaskManagerSteam.h"
#include "Private/OnlineLeaderboardInterfaceSteam.h"
#include "steam/isteamuserstats.h"

/**
*	Attach UGC to leaderboard
*
* @param LeaderboardName name of leaderboard
* @param StatName name of stat
*/
inline FName GetLeaderboardStatName(const FName& LeaderboardName, const FName& StatName)
{
	return TCHAR_TO_ANSI((*FString::Printf(TEXT("%s_%s"), *LeaderboardName.ToString(), *StatName.ToString())));
}

FOnlineAsyncTaskSteamWriteCloud::FOnlineAsyncTaskSteamWriteCloud(
	FOnlineSubsystemSteam* InSteamSubsystem,
	FAdvancedSteamCloud* InAdvancedSteamCloudPtr,
	IAdvancedSteamLeaderboardInterfacePtr InAdvancedSteamLeaderboardPtr,
	const FName& InLeaderboardName,
	const FName& InRatedStat,
	const FString& InUGCHandle,
	bool InShouldTriggerDelegates)
	:
	FOnlineAsyncTaskSteam(InSteamSubsystem, k_uAPICallInvalid),
	bInit(false),
	AdvancedSteamCloudPtr(InAdvancedSteamCloudPtr),
	AdvancedSteamLeaderboardPtr(InAdvancedSteamLeaderboardPtr),
	LeaderboardName(InLeaderboardName),
	RatedStat(InRatedStat),
	UGCHandle(FCString::Atoi64(*InUGCHandle)),
	ShouldTriggerDelegates(InShouldTriggerDelegates)
{
}

/**
*	Get a human readable description of task
*/
FString FOnlineAsyncTaskSteamWriteCloud::ToString() const 
{
	return FString::Printf(TEXT("FOnlineAsyncTaskSteamWriteCloud bWasSuccessful: %d Leaderboard: %s"), bWasSuccessful, *LeaderboardName.ToString());
}

/**
* Give the async task time to do its work
* Can only be called on the async task manager thread
*/
void FOnlineAsyncTaskSteamWriteCloud::Tick()
{
	ISteamUtils* SteamUtilsPtr = SteamUtils();
	check(SteamUtilsPtr);
	if (!bInit)
	{
		// Poll for leaderboard handle
		SteamLeaderboard_t LeaderboardHandle = -1;
		{
#if !WITH_EDITOR
			FLeaderboardMetadata* Leaderboard = AdvancedSteamLeaderboardPtr->GetLeaderboardMetadata(LeaderboardName);
			if (Leaderboard)
			{
				LeaderboardHandle = Leaderboard->LeaderboardHandle;
			}
#endif
		}

		if (LeaderboardHandle != -1)
		{
			const FString RatedStatName = GetLeaderboardStatName(LeaderboardName, RatedStat).ToString();
			// Stats are set here to ensure that this happens before any possible call to StoreStats()
			ISteamUserStats* SteamUserStatsPtr = SteamUserStats();
			check(SteamUserStatsPtr);
			// upload leaderboard score
			CallbackHandle = SteamUserStatsPtr->AttachLeaderboardUGC(LeaderboardHandle, UGCHandle);
		}
		bInit = true;
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
				((CallbackResults.m_eResult == 1) ? true : false);
		}
	}
	else
	{
		// Invalid API call
		bWasSuccessful = false;
		bIsComplete = true;
	}
}

void FOnlineAsyncTaskSteamWriteCloud::TriggerDelegates()
{
	FOnlineAsyncTaskSteam::TriggerDelegates();
#if !WITH_EDITOR
	if (ShouldTriggerDelegates)
	{
		AdvancedSteamCloudPtr->TriggerOnAttachFileCompleteDelegates(bWasSuccessful, ErrorCode);
	}
#endif
}
