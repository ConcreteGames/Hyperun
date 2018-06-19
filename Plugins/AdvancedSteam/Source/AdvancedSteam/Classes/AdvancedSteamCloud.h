// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Online.h"

#include "Interfaces/AdvancedSteamCloudInterface.h"
#include "Interfaces/OnlineSharedCloudInterface.h"

class ADVANCEDSTEAM_API FAdvancedSteamCloud : public IAdvancedSteamCloudInterface
{
public:
	FAdvancedSteamCloud();
	virtual ~FAdvancedSteamCloud();

	virtual bool WriteToCloud(const FName& LeaderboardName, const FName& StatName, const FString& UGCHandle) override;
};

typedef TSharedPtr<FAdvancedSteamCloud, ESPMode::ThreadSafe> FAdvancedSteamCloudPtr;