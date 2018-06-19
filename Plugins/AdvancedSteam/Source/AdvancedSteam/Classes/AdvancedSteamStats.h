// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Interfaces/AdvancedSteamStatsInterface.h"

class ADVANCEDSTEAM_API FAdvancedSteamStats : public IAdvancedSteamStatsInterface
{
public:
	FAdvancedSteamStats() {}
	virtual ~FAdvancedSteamStats() {}

	virtual bool GetStats(const FUniqueNetIdSteam& InUserId, FSteamStatsReadPtr InReadObject, bool ShouldTriggerRefDelegates) override;
	virtual bool SetStats(const FUniqueNetIdSteam& InUserId, FSteamStatsReadPtr InReadObject, bool InIncreaseStat) override;
};

typedef TSharedPtr<FAdvancedSteamStats, ESPMode::ThreadSafe> FAdvancedSteamStatsPtr;