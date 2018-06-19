// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/**
*	Async task to update stats for a single user from the Steam backend
*/
#include "AdvancedSteamPrivatePCH.h"
#include "OnlineAsyncTaskSteamUpdateUserStats.h"

#include "AdvancedSteamStats.h"
#include "OnlineSubsystemSteam.h"
#include "Private/OnlineAsyncTaskManagerSteam.h"


FOnlineAsyncTaskSteamUpdateUserStats::FOnlineAsyncTaskSteamUpdateUserStats(FOnlineSubsystemSteam* InSteamSubsystem, 
	const FUniqueNetIdSteam& InUserId, 
	FSteamStatsReadPtr InReadObject, 
	const bool InIncreaseStat, 
	bool InShouldTriggerDelegates, 
	FAdvancedSteamStats* InAdvancedSteamStats)
	:
	FOnlineAsyncTaskSteam(InSteamSubsystem, k_uAPICallInvalid),
	bInit(false),
	UserId(InUserId),
	ReadObject(InReadObject),
	IncreaseStat(InIncreaseStat),
	ShouldTriggerDelegates(InShouldTriggerDelegates),
	AdvancedSteamStats(InAdvancedSteamStats)
{
}

/**
*	Get a human readable description of task
*/
FString FOnlineAsyncTaskSteamUpdateUserStats::ToString() const 
{
	return FString::Printf(TEXT("FOnlineAsyncTaskSteamUpdateUserStats bWasSuccessful: %d UserId: %s"), bWasSuccessful, *UserId.ToDebugString());
}

/**
* Give the async task time to do its work
* Can only be called on the async task manager thread
*/
void FOnlineAsyncTaskSteamUpdateUserStats::Tick()
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
				CSteamID SteamUserId(*(uint64*)UserId.GetBytes());

				// Stats are set here to ensure that this happens before any possible call to StoreStats()
				ISteamUserStats* SteamUserStatsPtr = SteamUserStats();
				check(SteamUserStatsPtr);


				ErrorCode = EAdvancedSteamErrorCode::UNKNOWN;

				bool bSuccess = false;
				for (int32 StatIdx = 0; StatIdx < ReadObject->StatsArray.Num(); StatIdx++)
				{
					FSteamStats& StatInfo = ReadObject->StatsArray[StatIdx];
					switch (StatInfo.StatsType)
					{
					case EStatsType::Int32:
					{
						if (IncreaseStat)
						{
							int32 OldValue;
							if (SteamUserStatsPtr->GetUserStat(SteamUserId, TCHAR_TO_UTF8(*StatInfo.KeyName.ToString()), &OldValue))
							{
								bSuccess = SteamUserStatsPtr->SetStat(TCHAR_TO_UTF8(*StatInfo.KeyName.ToString()), OldValue + StatInfo.Int32Value);
								if (bSuccess)
								{
									StatInfo.Int32Value += OldValue;
								}
								else
								{
									ErrorCode = EAdvancedSteamErrorCode::STEAM_STATS_INVALID;
								}
							}
						}
						else
						{
							bSuccess = SteamUserStatsPtr->SetStat(TCHAR_TO_UTF8(*StatInfo.KeyName.ToString()), StatInfo.Int32Value);
							if (!bSuccess)
							{
								ErrorCode = EAdvancedSteamErrorCode::STEAM_STATS_INVALID;
							}
						}
						break;
					}
					case EStatsType::Float:
					{
						if (IncreaseStat)
						{
							float OldValue;
							if (SteamUserStatsPtr->GetUserStat(SteamUserId, TCHAR_TO_UTF8(*StatInfo.KeyName.ToString()), &OldValue))
							{
								bSuccess = SteamUserStatsPtr->SetStat(TCHAR_TO_UTF8(*StatInfo.KeyName.ToString()), OldValue + StatInfo.FloatValue);
								if (bSuccess)
								{
									StatInfo.FloatValue += OldValue;
								}
								else
								{
									ErrorCode = EAdvancedSteamErrorCode::STEAM_STATS_INVALID;
								}
							}
						}
						else
						{
							bSuccess = SteamUserStatsPtr->SetStat(TCHAR_TO_UTF8(*StatInfo.KeyName.ToString()), StatInfo.FloatValue);
							if (!bSuccess)
							{
								ErrorCode = EAdvancedSteamErrorCode::STEAM_STATS_INVALID;
							}
						}
						break;
					}
					default:
						UE_LOG_ONLINE(Warning, TEXT("Skipping unsuppported key value pair to Steam %s"), *StatInfo.KeyName.ToString());
						bSuccess = false;
						ErrorCode = EAdvancedSteamErrorCode::UNSUPPORTED_TYPE;
						break;
					}
					if (!bSuccess)
					{
						break;
					}
				}
				if (bSuccess)
				{
					if (!SteamUserStatsPtr->StoreStats())
					{
						UE_LOG_ONLINE(Warning, TEXT("Failure to store stats to Steam"));
						bWasSuccessful = false;
						ErrorCode = EAdvancedSteamErrorCode::STEAM_STATS_STORE_FAILED;
					}
				}
				else
				{
					UE_LOG_ONLINE(Warning, TEXT("Failure to get key value pair when getting to Steam"));
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
void FOnlineAsyncTaskSteamUpdateUserStats::Finalize()
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

void FOnlineAsyncTaskSteamUpdateUserStats::TriggerDelegates()
{
	FOnlineAsyncTaskSteam::TriggerDelegates();
#if !WITH_EDITOR
	if (ShouldTriggerDelegates)
	{
		AdvancedSteamStats->TriggerOnStatsWriteCompleteDelegates(bWasSuccessful, ErrorCode);
	}
#endif
}
