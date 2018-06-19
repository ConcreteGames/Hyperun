// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Private/OnlineSubsystemSteamTypes.h"
#include "OnlineSubsystemTypes.h"
#include "OnlineDelegateMacros.h"

#include "Interfaces/AdvancedSteamAchievementsInterface.h"
#include "Interfaces/AdvancedSteamCloudInterface.h"
#include "Interfaces/AdvancedSteamLeaderboardInterface.h"
#include "Interfaces/AdvancedSteamStatsInterface.h"

#include "AdvancedSteamAchievements.h"
#include "AdvancedSteamCloud.h"
#include "AdvancedSteamLeaderboard.h"
#include "AdvancedSteamStats.h"

#include "AdvancedSteam.generated.h"

/** Forward declarations of all interface classes */
typedef TSharedPtr<class FAdvancedSteamAchievements, ESPMode::ThreadSafe> FAdvancedSteamAchievementsPtr;
typedef TSharedPtr<class FAdvancedSteamCloud, ESPMode::ThreadSafe> FAdvancedSteamCloudPtr;
typedef TSharedPtr<class FAdvancedSteamLeaderboard, ESPMode::ThreadSafe> FAdvancedSteamLeaderboardPtr;
typedef TSharedPtr<class FAdvancedSteamStats, ESPMode::ThreadSafe> FAdvancedSteamStatsPtr;

UCLASS()
class ADVANCEDSTEAM_API UAdvancedSteam : public UObject
{
	GENERATED_UCLASS_BODY()

protected:
	/** Interface to steam stats */
	FAdvancedSteamAchievementsPtr SteamAchievementsInterface;
	FAdvancedSteamCloudPtr SteamCloudInterface;
	FAdvancedSteamLeaderboardPtr SteamLeaderboardInterface;
	FAdvancedSteamStatsPtr SteamStatsInterface;

public:
	bool Init();

	bool Shutdown();

	virtual void FinishDestroy() override;

	virtual IAdvancedSteamAchievementsInterfacePtr GetAdvancedSteamAchievementsInterface() const;
	virtual IAdvancedSteamCloudInterfacePtr GetAdvancedSteamCloudInterface() const;
	virtual IAdvancedSteamLeaderboardInterfacePtr GetAdvancedSteamLeaderboardInterface() const;
	virtual IAdvancedSteamStatsInterfacePtr GetAdvancedSteamStatsInterface() const;
};
