// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/**
*	Async task to update leaderboard for a single user from the Steam backend
*/
#include "AdvancedSteamPrivatePCH.h"
#include "OnlineAsyncTaskSteamUploadLeaderboard.h"

#include "AdvancedSteamLeaderboard.h"
#include "OnlineSubsystemSteam.h"
#include "Private/OnlineAsyncTaskManagerSteam.h"
#include "Private/OnlineLeaderboardInterfaceSteam.h"
#include "steam/isteamuserstats.h"

/**
*	Create the proper stat name for a given leaderboard/stat combination
*
* @param LeaderboardName name of leaderboard
* @param StatName name of stat
*/
extern inline FName GetLeaderboardStatName(const FName& LeaderboardName, const FName& StatName);

FOnlineAsyncTaskSteamUploadLeaderboard::FOnlineAsyncTaskSteamUploadLeaderboard(
	FOnlineSubsystemSteam* InSteamSubsystem,
	FAdvancedSteamLeaderboard* InAdvancedSteamLeaderboardPtr,
	ELeaderboardUploadMethod InLeaderboardUploadMethod,
	const FName& InLeaderboardName,
	const FName& InRatedStat,
	const int32 InNewScore,
	const TArray<int32>& InDetails,
	bool InShouldTriggerDelegates)
	:
	FOnlineAsyncTaskSteam(InSteamSubsystem, k_uAPICallInvalid),
	bInit(false),
	AdvancedSteamLeaderboardPtr(InAdvancedSteamLeaderboardPtr),
	LeaderboardUploadMethod(InLeaderboardUploadMethod),
	LeaderboardName(InLeaderboardName),
	RatedStat(InRatedStat),
	NewScore(InNewScore),
	Details(InDetails),
	ShouldTriggerDelegates(InShouldTriggerDelegates)
{
}

/**
*	Get a human readable description of task
*/
FString FOnlineAsyncTaskSteamUploadLeaderboard::ToString() const 
{
	return FString::Printf(TEXT("FOnlineAsyncTaskSteamUploadLeaderboard bWasSuccessful: %d Leaderboard: %s Score: %d"), bWasSuccessful, *LeaderboardName.ToString(), NewScore);
}

/**
* Give the async task time to do its work
* Can only be called on the async task manager thread
*/
void FOnlineAsyncTaskSteamUploadLeaderboard::Tick()
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
			bool bSuccess = false;

			// set steam leaderboard details
			int* LeaderboardDetails = nullptr;
			int DetailCount = 0;
			if (Details.Num() > 0)
			{
				LeaderboardDetails = Details.GetData();
				DetailCount = Details.Num();
			}

			ELeaderboardUploadScoreMethod UpdateMethodSteam;
			switch (LeaderboardUploadMethod)
			{
			case ELeaderboardUploadMethod::ELeaderboardUploadMethodForceUpdate:
				UpdateMethodSteam = k_ELeaderboardUploadScoreMethodForceUpdate;
				break;
			case ELeaderboardUploadMethod::ELeaderboardUploadMethodKeepBest:
			default:
				UpdateMethodSteam = k_ELeaderboardUploadScoreMethodKeepBest;
				break;
			}

			// upload leaderboard score
			CallbackHandle = SteamUserStatsPtr->UploadLeaderboardScore(LeaderboardHandle, UpdateMethodSteam, NewScore, LeaderboardDetails, DetailCount);
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
				((CallbackResults.m_bSuccess != 0) ? true : false);
		}
	}
	else
	{
		// Invalid API call
		bWasSuccessful = false;
		bIsComplete = true;
	}
}

void FOnlineAsyncTaskSteamUploadLeaderboard::TriggerDelegates()
{
	FOnlineAsyncTaskSteam::TriggerDelegates();
#if !WITH_EDITOR
	if (ShouldTriggerDelegates)
	{
		AdvancedSteamLeaderboardPtr->TriggerOnScoreUploadCompleteDelegates(bWasSuccessful, ErrorCode);
	}
#endif
}
