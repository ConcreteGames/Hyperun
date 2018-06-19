// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once
/**
*	Async task to update leaderboard for a single user from the Steam backend
*/

#include "Interfaces/AdvancedSteamLeaderboardInterface.h"
#include "Interfaces/OnlineLeaderboardInterface.h"
#include "Private/OnlineSubsystemSteamTypes.h"

#include "Private/OnlineAsyncTaskManagerSteam.h"
#include "SteamLeaderboard.h"

class FOnlineAsyncTaskSteam;
class FOnlineSubsystemSteam;
class FAdvancedSteamCloud;

class FOnlineAsyncTaskSteamWriteCloud : public FOnlineAsyncTaskSteam
{
private:

	/** Has this request been started */
	bool bInit;
	/** Name of leaderboard to update */
	FName LeaderboardName;
	/** Name of stat that will replace/update the existing value on the leaderboard */
	FName RatedStat;
	/** UGC handle */
	UGCHandle_t UGCHandle;
	/** Results returned from Steam backend */
	LeaderboardUGCSet_t CallbackResults;
	/** Since we can write multiple leaderboards with one call, indicate whether this is the last one */
	bool ShouldTriggerDelegates;

	EAdvancedSteamErrorCode ErrorCode;
	
	/** Potentially multiple user requests involved in filling in the read object, should this one trigger the finished delegate */
	IAdvancedSteamLeaderboardInterfacePtr AdvancedSteamLeaderboardPtr;
	FAdvancedSteamCloud* AdvancedSteamCloudPtr;

	/** Hidden on purpose */
	FOnlineAsyncTaskSteamWriteCloud();

public:

	FOnlineAsyncTaskSteamWriteCloud(
		FOnlineSubsystemSteam* InSteamSubsystem,
		FAdvancedSteamCloud* InAdvancedSteamCloudPtr,
		IAdvancedSteamLeaderboardInterfacePtr InAdvancedSteamLeaderboardPtr,
		const FName& InLeaderboardName,
		const FName& InRatedStat,
		const FString& InUGCHandle,
		bool InShouldTriggerDelegates
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
	virtual void TriggerDelegates() override;
};