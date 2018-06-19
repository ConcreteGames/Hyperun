// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/**
*	Async task to update achievement for a single user from the Steam backend
*/
#include "AdvancedSteamPrivatePCH.h"
#include "OnlineAsyncTaskSteamUpdateAchievements.h"

#include "AdvancedSteamAchievements.h"
#include "OnlineSubsystemSteam.h"
#include "Private/OnlineAsyncTaskManagerSteam.h"

FOnlineAsyncTaskSteamUpdateAchievements::FOnlineAsyncTaskSteamUpdateAchievements(
	FOnlineSubsystemSteam* InSteamSubsystem,
	const FUniqueNetIdSteam& InUserId,
	const FName& InAchievementId,
	bool InClearAchievement,
	bool InShouldTriggerDelegates,
	FAdvancedSteamAchievements* InAdvancedSteamAchievements
	)
	:
	FOnlineAsyncTaskSteam(InSteamSubsystem, k_uAPICallInvalid),
	bInit(false),
	UserId(InUserId),
	AchievementId(InAchievementId),
	ClearAchievement(InClearAchievement),
	ShouldTriggerDelegates(InShouldTriggerDelegates),
	AdvancedSteamAchievements(InAdvancedSteamAchievements)
{
}

/**
*	Get a human readable description of task
*/
FString FOnlineAsyncTaskSteamUpdateAchievements::ToString() const 
{
	return FString::Printf(TEXT("FOnlineAsyncTaskSteamUpdateAchievements bWasSuccessful: %d UserId: %s"), bWasSuccessful, *UserId.ToDebugString());
}

/**
* Give the async task time to do its work
* Can only be called on the async task manager thread
*/
void FOnlineAsyncTaskSteamUpdateAchievements::Tick()
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
				// Stats are set here to ensure that this happens before any possible call to StoreStats()
				ISteamUserStats* SteamUserStatsPtr = SteamUserStats();
				check(SteamUserStatsPtr);

				bool bSuccess = false;
				if (ClearAchievement)
				{
#if UE_BUILD_SHIPPING
					bSuccess = true;
#else
					bSuccess = SteamUserStatsPtr->ClearAchievement(TCHAR_TO_UTF8(*AchievementId.ToString()));
#endif
				}
				else
				{
					bSuccess = SteamUserStatsPtr->SetAchievement(TCHAR_TO_UTF8(*AchievementId.ToString()));
				}

				if (bSuccess)
				{
					if (!SteamUserStatsPtr->StoreStats())
					{
						UE_LOG_ONLINE(Warning, TEXT("Failure to store stats when updating achievement(%s)"), *AchievementId.ToString());
						bWasSuccessful = false;
						ErrorCode = EAdvancedSteamErrorCode::STEAM_STATS_STORE_FAILED;
					}
				}
				else
				{
					UE_LOG_ONLINE(Warning, TEXT("Failure to store stats when updating achievement(%s)"), *AchievementId.ToString());
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
void FOnlineAsyncTaskSteamUpdateAchievements::Finalize()
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

void FOnlineAsyncTaskSteamUpdateAchievements::TriggerDelegates()
{
	FOnlineAsyncTaskSteam::TriggerDelegates();
#if !WITH_EDITOR
	if (ShouldTriggerDelegates)
	{
		AdvancedSteamAchievements->TriggerOnAchievementsUpdateCompleteDelegates(bWasSuccessful, ErrorCode);
	}
#endif
}