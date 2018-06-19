// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/**
*	Async task to retrieve all achievements for a single user from the Steam backend
*/
#include "AdvancedSteamPrivatePCH.h"
#include "OnlineAsyncTaskSteamRetrieveAchievements.h"

#include "AdvancedSteamAchievements.h"
#include "OnlineSubsystemSteam.h"
#include "Private/OnlineAsyncTaskManagerSteam.h"

FOnlineAsyncTaskSteamRetrieveAchievements::FOnlineAsyncTaskSteamRetrieveAchievements(
	FOnlineSubsystemSteam* InSteamSubsystem,
	const FUniqueNetIdSteam& InUserId,
	bool InShouldTriggerDelegates, 
	FAdvancedSteamAchievements* InAdvancedSteamAchievements)
	:
	FOnlineAsyncTaskSteam(InSteamSubsystem, k_uAPICallInvalid),
	bInit(false),
	UserId(InUserId),
	ShouldTriggerDelegates(InShouldTriggerDelegates),
	AdvancedSteamAchievements(InAdvancedSteamAchievements)
{
}

/**
*	Get a human readable description of task
*/
FString FOnlineAsyncTaskSteamRetrieveAchievements::ToString() const 
{
	return FString::Printf(TEXT("FOnlineAsyncTaskSteamRetrieveAchievements bWasSuccessful: %d UserId: %s"), bWasSuccessful, *UserId.ToDebugString());
}

/**
* Give the async task time to do its work
* Can only be called on the async task manager thread
*/
void FOnlineAsyncTaskSteamRetrieveAchievements::Tick()
{
	if (!bInit)
	{
		ISteamUserStats* SteamUserStatsPtr = SteamUserStats();
		check(SteamUserStatsPtr);
		CallbackHandle = SteamUserStatsPtr->RequestUserStats(*(uint64*)UserId.GetBytes());
		bInit = true;
	}

	if (CallbackHandle != k_uAPICallInvalid)
	{
		ISteamUtils* SteamUtilsPtr = SteamUtils();
		check(SteamUtilsPtr);
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
				(!bFailedResult ? true : false);

			if (bWasSuccessful)
			{
				if (!AdvancedSteamAchievements->UpdateAchievementsForUser(UserId, ErrorCode))
				{
					bWasSuccessful = false;
				}
			}
		}
	}
	else
	{
		bWasSuccessful = false;
		bIsComplete = false;
		ErrorCode = EAdvancedSteamErrorCode::UNKNOWN;
	}
}

/**
* Give the async task a chance to marshal its data back to the game thread
* Can only be called on the game thread by the async task manager
*/
void FOnlineAsyncTaskSteamRetrieveAchievements::Finalize()
{
	FOnlineAsyncTaskSteam::Finalize();
	
	if (bWasSuccessful)
	{
		const CGameID GameID(Subsystem->GetSteamAppId());
		if (GameID.ToUint64() == CallbackResults.m_nGameID)
		{
			check(UserId == FUniqueNetIdSteam(CallbackResults.m_steamIDUser));
			if (CallbackResults.m_eResult != k_EResultOK)
			{
				ErrorCode = EAdvancedSteamErrorCode::INVALID_GAMEID;
				if (CallbackResults.m_eResult == k_EResultFail)
				{
					UE_LOG_ONLINE(Warning, TEXT("Failed to obtain steam user stats, user: %s has no stats entries"), *UserId.ToDebugString());
				}
				else
				{
					UE_LOG_ONLINE(Warning, TEXT("Failed to obtain steam user stats, user: %s error: %d"), *UserId.ToDebugString(), (int32)CallbackResults.m_eResult);
				}
			}

			bWasSuccessful = (CallbackResults.m_eResult == k_EResultOK) ? true : false;
		}
		else
		{
			UE_LOG_ONLINE(Warning, TEXT("Obtained steam user stats, but for wrong game! Ignoring."));
		}
	}
	else
	{
		UE_LOG_ONLINE(Warning, TEXT("Failed to obtain steam user stats, user: %s error: unknown"), *UserId.ToDebugString());
	}
}

void FOnlineAsyncTaskSteamRetrieveAchievements::TriggerDelegates()
{
	FOnlineAsyncTaskSteam::TriggerDelegates();
#if !WITH_EDITOR
	if (ShouldTriggerDelegates)
	{		
		AdvancedSteamAchievements->TriggerOnAchievementsReadCompleteDelegates(bWasSuccessful, ErrorCode);
	}
#endif
}
