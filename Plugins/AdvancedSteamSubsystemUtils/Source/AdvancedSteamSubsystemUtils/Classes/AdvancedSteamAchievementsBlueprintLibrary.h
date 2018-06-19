// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "SteamAchievements.h"
#include "AdvancedSteamAchievementsBlueprintLibrary.generated.h"

UCLASS()
class ADVANCEDSTEAMSUBSYSTEMUTILS_API UAdvancedSteamAchievementsBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/** Return cached ahievement. This function should be called after retrieving achievements */
	UFUNCTION(BlueprintCallable, Category = "Online|SteamAchievements")
	static bool GetCachedAchievement(FName AchievementId, FSteamAchievements& OutAchievement);
	
	/** Update stat value if it is progress stat. */
	UFUNCTION(BlueprintCallable, Category = "Online|SteamAchievements")
	static bool SetProgress(FName AchievementId, FSteamStats StatInfo);

	/** Return achievements icon */
	UFUNCTION(BlueprintCallable, Category = "Online|SteamAchievements")
	static UTexture2D* GetAchievementIcon(int32 Image);
};
