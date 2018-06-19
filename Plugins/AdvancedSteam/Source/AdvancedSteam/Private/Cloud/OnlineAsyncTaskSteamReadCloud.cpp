// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/**
*	Async task to update leaderboard for a single user from the Steam backend
*/
#include "AdvancedSteamPrivatePCH.h"
#include "OnlineAsyncTaskSteamReadCloud.h"

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
extern inline FName GetLeaderboardStatName(const FName& LeaderboardName, const FName& StatName);

FOnlineAsyncTaskSteamReadCloud::FOnlineAsyncTaskSteamReadCloud(
	FOnlineSubsystemSteam* InSteamSubsystem,
	FAdvancedSteamCloud* InAdvancedSteamCloudPtr,
	const FName& InLeaderboardName,
	const FName& InRatedStat,
	const FString& InUGCHandle,
	bool InShouldTriggerDelegates)
	:
	FOnlineAsyncTaskSteam(InSteamSubsystem, k_uAPICallInvalid),
	bInit(false),
	AdvancedSteamCloudPtr(InAdvancedSteamCloudPtr),
	LeaderboardName(InLeaderboardName),
	RatedStat(InRatedStat),
	UGCHandle(FCString::Atoi64(*InUGCHandle)),
	ShouldTriggerDelegates(InShouldTriggerDelegates)
{
}

/**
*	Get a human readable description of task
*/
FString FOnlineAsyncTaskSteamReadCloud::ToString() const 
{
	return FString::Printf(TEXT("FOnlineAsyncTaskSteamReadCloud bWasSuccessful: %d Leaderboard: %s"), bWasSuccessful, *LeaderboardName.ToString());
}

/**
* Give the async task time to do its work
* Can only be called on the async task manager thread
*/
void FOnlineAsyncTaskSteamReadCloud::Tick()
{
	ISteamUtils* SteamUtilsPtr = SteamUtils();
	check(SteamUtilsPtr);
	if (!bInit)
	{
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

void FOnlineAsyncTaskSteamReadCloud::TriggerDelegates()
{
	FOnlineAsyncTaskSteam::TriggerDelegates();
#if !WITH_EDITOR
	if (ShouldTriggerDelegates)
	{
		AdvancedSteamCloudPtr->TriggerOnAttachFileCompleteDelegates(bWasSuccessful, ErrorCode);
	}
#endif
}
