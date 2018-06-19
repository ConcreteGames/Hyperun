// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SteamAchievements.h"
#include "Online.h"

#include "Interfaces/AdvancedSteamAchievementsInterface.h"


class FAdvancedSteamAchievements : public IAdvancedSteamAchievementsInterface
{
public:
	FAdvancedSteamAchievements();
	virtual ~FAdvancedSteamAchievements();

	/** Retrieve achievements by asychronized call */
	virtual bool QueryAchievements(const FUniqueNetIdSteam& InUserId) override;

	/** Return specific player's ahievements. This function should be called after retrieving achievements */
	virtual bool GetAchievements(const FUniqueNetIdSteam& InUserId, TArray<FSteamAchievements>& OutAchievementsArray) override;

	/** Return cached ahievement. This function should be called after retrieving achievements */
	virtual bool GetCachedAchievement(const FUniqueNetIdSteam& InUserId, const FName& AchievementId, FSteamAchievements& OutAchievement) override;

	/** Update stat value if it is progress stat. */
	virtual bool SetProgress(const FUniqueNetIdSteam& InUserId, const FName& AchievementId, const FSteamStats& StatInfo) override;

	/** Triggering an achievement */
	virtual bool SetAchievement(const FUniqueNetIdSteam& InUserId, const FName& AchievementId) override;
	virtual bool ClearAchievement(const FUniqueNetIdSteam& InUserId, const FName& AchievementId) override;

	/** Return Steam Image by index */
	virtual UTexture2D* GetAchievementIcon(const int32 Image) override;

	/** Update ahievements information when . This function should be called after retrieving achievements */
	bool UpdateAchievementsForUser(const FUniqueNetIdSteam& PlayerId, EAdvancedSteamErrorCode &ErrorCode);

protected:
	/** Achievements write object */
	FOnlineAchievementsWritePtr WriteObject;

	/** Mapping of players to their achievements */
	TMap<FUniqueNetIdSteam, TArray<FSteamAchievementsPtr>> PlayerAchievements;

	/** Achievements configured in the config (not player-specific) */
	TArray<FSteamAchievementsPtr> Achievements;
	bool ReadAchievements(TArray<FSteamAchievementsPtr> &OutArray);

	/** Returns empty string if couldn't read */
	FString GetKey(const FString& KeyName);

	/** Return steam image to UTexture2D */
	UTexture2D* GetSteamImageAsTexture(int32 Image);

	TMap<int, UTexture2D*> IconImageMap;
};