// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OnlineDelegateMacros.h"
#include "OnlineSubsystemTypes.h"
#include "OnlineStats.h"
#include "Private/OnlineSubsystemSteamTypes.h"
#include "SteamAchievements.h"

/**
* Delegate called when the last stats read is complete
*
* @param bWasSuccessful was the stats read successful or not
* @param ErrorCode was error code when it failed
*/

#if !WITH_EDITOR

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAchievementsReadComplete, bool, const EAdvancedSteamErrorCode&);
typedef FOnAchievementsReadComplete::FDelegate FOnAchievementsReadCompleteDelegate;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAchievementsUpdateComplete, bool, const EAdvancedSteamErrorCode&);
typedef FOnAchievementsUpdateComplete::FDelegate FOnAchievementsUpdateCompleteDelegate;

#endif

class IAdvancedSteamAchievementsInterface
{
protected:
	IAdvancedSteamAchievementsInterface() {}

public:
	virtual ~IAdvancedSteamAchievementsInterface() {}

	virtual bool QueryAchievements(const FUniqueNetIdSteam& InUserId) = 0;
	virtual bool GetAchievements(const FUniqueNetIdSteam& InUserId, TArray<FSteamAchievements>& OutAchievementsArray) = 0;
	virtual bool GetCachedAchievement(const FUniqueNetIdSteam& PlayerId, const FName& AchievementId, FSteamAchievements& OutAchievement) = 0;
	virtual bool SetProgress(const FUniqueNetIdSteam& InUserId, const FName& AchievementId, const FSteamStats& StatInfo) = 0;
	virtual bool SetAchievement(const FUniqueNetIdSteam& InUserId, const FName& AchievementId) = 0;
	virtual bool ClearAchievement(const FUniqueNetIdSteam& InUserId, const FName& AchievementId) = 0;
	virtual UTexture2D* GetAchievementIcon(const int32 Image) = 0;
#if !WITH_EDITOR
	/**
	* Notifies the interested party that the last stats read has completed
	*
	* @param bWasSuccessful true if the async action completed without error, false if there was an error
	*/

	DEFINE_ONLINE_DELEGATE_TWO_PARAM(OnAchievementsReadComplete, bool, const EAdvancedSteamErrorCode&);
	DEFINE_ONLINE_DELEGATE_TWO_PARAM(OnAchievementsUpdateComplete, bool, const EAdvancedSteamErrorCode&);
#endif

};

typedef TSharedPtr<IAdvancedSteamAchievementsInterface, ESPMode::ThreadSafe> IAdvancedSteamAchievementsInterfacePtr;