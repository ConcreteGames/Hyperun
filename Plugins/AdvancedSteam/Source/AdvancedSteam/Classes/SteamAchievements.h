// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"
#include "SteamStats.h"

#include "SteamAchievements.generated.h"


USTRUCT(BlueprintType)
struct FSteamAchievements
{
	GENERATED_USTRUCT_BODY()

public:
	FSteamAchievements()
	{
	}

	FSteamAchievements(const FSteamAchievements& InAchievementInfo)
	{
		SetValue(InAchievementInfo);
	}

	void SetValue(const FSteamAchievements& InAchievementInfo)
	{
		AchievementId = InAchievementInfo.AchievementId;
		StatInfo = InAchievementInfo.StatInfo;
		IsProgressAchievement = InAchievementInfo.IsProgressAchievement;
		IsCleared = InAchievementInfo.IsCleared;
		IconImage = InAchievementInfo.IconImage;
		UnLocked = InAchievementInfo.UnLocked;
		Title = InAchievementInfo.Title;
		Desc = InAchievementInfo.Desc;
		IsHidden = InAchievementInfo.IsHidden;
		UnlockTime = InAchievementInfo.UnlockTime;
	}

	/** Achievement id */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SteamAchievements)
	FName AchievementId;

	/** If IsProgressAchievement is true, StatInfo is valid. If not, only Unlocked variable is used. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SteamAchievements)
	bool IsProgressAchievement;

	/** Stat information for progressing achievement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SteamAchievements)
	FSteamStats StatInfo;

	/** Achievement cleared or not */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SteamAchievements)
	bool IsCleared;

	/** Achievement icon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SteamAchievements)
	int32 IconImage;

	/** Achievement lock or unlocked */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SteamAchievements)
	bool UnLocked;

	/** The localized title of the achievement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SteamAchievements)
	FText Title;

	/** The localized description of the achievement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SteamAchievements)
	FText Desc;

	/** Flag for whether the achievement is hidden */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SteamAchievements)
	bool IsHidden;

	/** The date/time the achievement was unlocked */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SteamAchievements)
	FDateTime UnlockTime;
};

typedef TSharedRef<FSteamAchievements, ESPMode::ThreadSafe> FSteamAchievementsRef;
typedef TSharedPtr<FSteamAchievements, ESPMode::ThreadSafe> FSteamAchievementsPtr;