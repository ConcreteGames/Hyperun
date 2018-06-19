// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once
/**
*	Async task to retrieve all achievements for a single user from the Steam backend
*/

#include "ErrorCodes.h"
#include "Private/OnlineSubsystemSteamTypes.h"
#include "Private/OnlineAsyncTaskManagerSteam.h"

class FOnlineAsyncTaskSteam;
class FAdvancedSteamAchievements;

class FOnlineAsyncTaskSteamRetrieveAchievements : public FOnlineAsyncTaskSteam
{
private:

	/** Has this task been initialized yet */
	bool bInit;
	/** User Id we are requesting stats for */
	FUniqueNetIdSteam UserId;
	/** Returned results from Steam */
	UserStatsReceived_t CallbackResults;

	/** Errorcodes for debug */
	EAdvancedSteamErrorCode ErrorCode;

	/** Potentially multiple user requests involved in filling in the read object, should this one trigger the finished delegate */
	bool ShouldTriggerDelegates;
	FAdvancedSteamAchievements* AdvancedSteamAchievements;

	/** Hidden on purpose */
	FOnlineAsyncTaskSteamRetrieveAchievements() {};

public:

	FOnlineAsyncTaskSteamRetrieveAchievements(
		FOnlineSubsystemSteam* InSteamSubsystem, 
		const FUniqueNetIdSteam& InUserId, 
		bool InShouldTriggerDelegates, 
		FAdvancedSteamAchievements* InAdvancedSteamAchievements
	);

	/**
	*	Get a human readable description of task
	*/
	FString ToString() const override;

	/**
	* Give the async task time to do its work
	* Can only be called on the async task manager thread
	*/
	virtual void Tick() override;

	/**
	* Give the async task a chance to marshal its data back to the game thread
	* Can only be called on the game thread by the async task manager
	*/
	virtual void Finalize() override;

	virtual void TriggerDelegates() override;
};