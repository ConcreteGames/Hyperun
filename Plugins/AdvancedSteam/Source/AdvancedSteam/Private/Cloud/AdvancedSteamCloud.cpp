// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AdvancedSteamPrivatePCH.h"
#include "AdvancedSteamCloud.h"

#include "AdvancedSteam.h"
#include "Interfaces/AdvancedSteamLeaderboardInterface.h"
#include "OnlineSubsystemSteam.h"
#include "Private/OnlineAsyncTaskManagerSteam.h"
#include "OnlineAsyncTaskSteamWriteCloud.h"

FAdvancedSteamCloud::FAdvancedSteamCloud() 
{
}

FAdvancedSteamCloud::~FAdvancedSteamCloud()
{
}

bool FAdvancedSteamCloud::WriteToCloud(const FName& LeaderboardName, const FName& StatName, const FString& UGCHandle)
{
#if WITH_EDITOR
	return true;
#else
	// Find or create handles to all requested Clouds (async)
	// Will create or retrieve the leaderboard, triggering async calls as appropriate
	UAdvancedSteam* AdvancedSteam = Cast<UAdvancedSteam>(UAdvancedSteam::StaticClass()->GetDefaultObject());
	if (AdvancedSteam)
	{
		IAdvancedSteamLeaderboardInterfacePtr AdvancedSteamLeaderboardInterface = AdvancedSteam->GetAdvancedSteamLeaderboardInterface();
		if (AdvancedSteamLeaderboardInterface.IsValid())
		{
			AdvancedSteamLeaderboardInterface->GetLeaderboardMetadata(LeaderboardName, true);
		}
		else
		{
			FFrame::KismetExecutionMessage(TEXT("WriteToCloud - Invalid or uninitialized AdvancedSteam"), ELogVerbosity::Warning);
		}
	}	

	FOnlineSubsystemSteam* OnlineSub = (FOnlineSubsystemSteam*)IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
	if (OnlineSub)
	{
		FOnlineAsyncTaskSteamWriteCloud* NewLeaderboardTask = new FOnlineAsyncTaskSteamWriteCloud(OnlineSub, this, AdvancedSteam->GetAdvancedSteamLeaderboardInterface(), LeaderboardName, StatName, UGCHandle, true);
		OnlineSub->QueueAsyncTask(NewLeaderboardTask);
		return true;
	}
	else
	{
		FFrame::KismetExecutionMessage(TEXT("WriteToCloud - Invalid or uninitialized OnlineSubsystem"), ELogVerbosity::Warning);
	}

	return false;
#endif
}
