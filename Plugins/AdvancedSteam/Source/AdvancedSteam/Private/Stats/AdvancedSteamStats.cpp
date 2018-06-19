// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AdvancedSteamPrivatePCH.h"
#include "AdvancedSteamStats.h"

#include "OnlineSubsystemSteam.h"
#include "Private/OnlineAsyncTaskManagerSteam.h"
#include "OnlineAsyncTaskSteamGetUserStats.h"
#include "OnlineAsyncTaskSteamUpdateUserStats.h"

bool FAdvancedSteamStats::GetStats(const FUniqueNetIdSteam& InUserId, FSteamStatsReadPtr ReadObject, bool ShouldTriggerRefDelegates)
{
#if WITH_EDITOR
	return true;
#else
	FOnlineSubsystemSteam* OnlineSub = (FOnlineSubsystemSteam*)IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
	if (OnlineSub)
	{
		FOnlineAsyncTaskSteamGetUserStats* NewStatsTask = new FOnlineAsyncTaskSteamGetUserStats(OnlineSub, InUserId, ReadObject, true, ShouldTriggerRefDelegates, this);
		OnlineSub->QueueAsyncTask(NewStatsTask);
		return true;
	}
	else
	{
		FFrame::KismetExecutionMessage(TEXT("GetStats - Invalid or uninitialized OnlineSubsystem"), ELogVerbosity::Warning);
	}

	return false;
#endif
}

bool FAdvancedSteamStats::SetStats(const FUniqueNetIdSteam& InUserId, FSteamStatsReadPtr InReadObject, bool InIncreaseStat)
{
#if WITH_EDITOR
	return true;
#else
	FOnlineSubsystemSteam* OnlineSub = (FOnlineSubsystemSteam*)IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
	if (OnlineSub)
	{		
		FOnlineAsyncTaskSteamUpdateUserStats* NewStatsTask = new FOnlineAsyncTaskSteamUpdateUserStats(OnlineSub, InUserId, InReadObject, InIncreaseStat, false, this);
		OnlineSub->QueueAsyncTask(NewStatsTask);
		return true;
	}
	else
	{
		FFrame::KismetExecutionMessage(TEXT("GetStats - Invalid or uninitialized OnlineSubsystem"), ELogVerbosity::Warning);
	}

	return false;
#endif
}
