// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "SteamLeaderboard.h"
#include "AdvancedSteamLeaderboardBlueprintLibrary.generated.h"

UCLASS()
class ADVANCEDSTEAMSUBSYSTEMUTILS_API UAdvancedSteamLeaderboardBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/** Writes an integer value to the specified leaderboard */
	UFUNCTION(BlueprintCallable, Category = "Online|SteamLeaderboard")
	static bool WriteSteamLeaderboardInteger(
		ELeaderboardUploadMethod LeaderboardUploadMethod,	/** ELeaderboardUploadMethodKeepBest will keep user's best score. ELeaderboardUploadMethodForceUpdate will always replace score with specified */
		FName LeaderboardName,								/** it will be created if it doesn't exist on steam leaderboard */ 
		FName StatName,										/** LeaderboardName_StatName is actual stat name in steam stats */
		int32 StatValue,									/** the value is updated in steam stats */
		TArray<int32> Details								/** Details are extra game-defined information regarding how the user got that score */
	);

	/** Return achievements icon */
	UFUNCTION(BlueprintCallable, Category = "Online|SteamLeaderboard")
	static UTexture2D* GetAvatarIcon(int32 Image);
};
