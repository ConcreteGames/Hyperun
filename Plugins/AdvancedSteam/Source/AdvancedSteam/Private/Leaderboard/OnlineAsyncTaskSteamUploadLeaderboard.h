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
class FAdvancedSteamLeaderboard;

class FOnlineAsyncTaskSteamUploadLeaderboard : public FOnlineAsyncTaskSteam
{
private:

	/** Has this request been started */
	bool bInit;
	/** Name of leaderboard to update */
	FName LeaderboardName;
	/** Name of stat that will replace/update the existing value on the leaderboard */
	FName RatedStat;
	/** Score that will replace/update the existing value on the leaderboard */
	int32 NewScore;
	/** Method of update against the previous score */
	ELeaderboardUpdateMethod::Type UpdateMethod;
	/** Results returned from Steam backend */
	LeaderboardScoreUploaded_t CallbackResults;
	/** Since we can write multiple leaderboards with one call, indicate whether this is the last one */
	bool ShouldTriggerDelegates;
	/** leaderboard details is  an array of int32's */
	TArray<int32> Details;

	EAdvancedSteamErrorCode ErrorCode;
	ELeaderboardUploadMethod LeaderboardUploadMethod;
	
	/** Potentially multiple user requests involved in filling in the read object, should this one trigger the finished delegate */
	FAdvancedSteamLeaderboard* AdvancedSteamLeaderboardPtr;

	/** Hidden on purpose */
	FOnlineAsyncTaskSteamUploadLeaderboard();

public:

	FOnlineAsyncTaskSteamUploadLeaderboard(
		FOnlineSubsystemSteam* InSteamSubsystem,
		FAdvancedSteamLeaderboard* InAdvancedSteamLeaderboardPtr,
		ELeaderboardUploadMethod LeaderboardUploadMethod,
		const FName& InLeaderboardName,
		const FName& InRatedStat,
		const int32 InNewScore,
		const TArray<int32>& InDetails,
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