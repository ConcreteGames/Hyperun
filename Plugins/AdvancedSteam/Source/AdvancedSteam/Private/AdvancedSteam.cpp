// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AdvancedSteamPrivatePCH.h"
#include "AdvancedSteam.h"

UAdvancedSteam::UAdvancedSteam(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Init();
}

bool UAdvancedSteam::Init()
{
	SteamAchievementsInterface = MakeShareable(new FAdvancedSteamAchievements());
	SteamCloudInterface = MakeShareable(new FAdvancedSteamCloud());
	SteamLeaderboardInterface = MakeShareable(new FAdvancedSteamLeaderboard());
	SteamStatsInterface = MakeShareable(new FAdvancedSteamStats());

	// Kick off a download/cache of the current user's stats
#if !WITH_EDITOR
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
	if (OnlineSub)
	{
		ISteamUserStats* SteamUserStatsPtr = SteamUserStats();
		if (SteamUserStatsPtr)
		{
			SteamUserStatsPtr->RequestCurrentStats();
		}
	}
#endif

	return true;
}

bool UAdvancedSteam::Shutdown()
{
	if (SteamAchievementsInterface.IsValid())
	{
		SteamAchievementsInterface = nullptr;
	}

	if (SteamCloudInterface.IsValid())
	{
		SteamCloudInterface = nullptr;
	}

	if (SteamStatsInterface.IsValid())
	{
		SteamStatsInterface = nullptr;
	}

	if (SteamLeaderboardInterface.IsValid())
	{
		SteamLeaderboardInterface = nullptr;
	}

	return true;
}

void UAdvancedSteam::FinishDestroy()
{
	Shutdown();

	Super::FinishDestroy();
}

IAdvancedSteamAchievementsInterfacePtr UAdvancedSteam::GetAdvancedSteamAchievementsInterface() const
{
	return SteamAchievementsInterface;
}

IAdvancedSteamCloudInterfacePtr UAdvancedSteam::GetAdvancedSteamCloudInterface() const
{
	return SteamCloudInterface;
}

IAdvancedSteamStatsInterfacePtr UAdvancedSteam::GetAdvancedSteamStatsInterface() const
{
	return SteamStatsInterface;
}

IAdvancedSteamLeaderboardInterfacePtr UAdvancedSteam::GetAdvancedSteamLeaderboardInterface() const
{
	return SteamLeaderboardInterface;
}