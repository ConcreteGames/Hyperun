// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once
/**
*	Async task to retrieve leaderboard entries for a single user from the Steam backend
*/

#include "Interfaces/AdvancedSteamLeaderboardInterface.h"
#include "Interfaces/OnlineLeaderboardInterface.h"
#include "Private/OnlineSubsystemSteamTypes.h"

#include "Private/OnlineAsyncTaskManagerSteam.h"
#include "SteamLeaderboard.h"

class FOnlineAsyncTaskSteam;
class FOnlineSubsystemSteam;
class FAdvancedSteamLeaderboard;

class FOnlineAsyncTaskSteamGetLeaderboardEntries : public FOnlineAsyncTaskSteam
{
private:

	/** Has this request been started */
	bool bInit;
	/** Players to request leaderboard data for */
	TArray< TSharedRef<const FUniqueNetId> > Players;
	/** Handle to the read object where the data will be stored */
	FSteamLeaderboardReadRef ReadObject;
	/** Results from callback */
	LeaderboardScoresDownloaded_t CallbackResults;

	EAdvancedSteamErrorCode ErrorCode;
	ELeaderboardRequest LeaderboardRequest;
	int32 RangeStart;
	int32 RangeEnd;
	int32 DetailCount;
	
	bool ShouldTriggerDelegates;

	/** Potentially multiple user requests involved in filling in the read object, should this one trigger the finished delegate */
	FAdvancedSteamLeaderboard* AdvancedSteamLeaderboardPtr;

	/** Hidden on purpose */
	FOnlineAsyncTaskSteamGetLeaderboardEntries();

public:

	FOnlineAsyncTaskSteamGetLeaderboardEntries(
		FOnlineSubsystemSteam* InSteamSubsystem, 
		const TArray< TSharedRef<const FUniqueNetId> >& InPlayers, 
		const FSteamLeaderboardReadRef& InReadObject, 
		FAdvancedSteamLeaderboard* InAdvancedSteamLeaderboardPtr,
		ELeaderboardRequest InLeaderboardDataRequest, 
		int32 InRangeStart, 
		int32 InRangeEnd, 
		int32 InDetailCount,
		bool InShouldTriggerDelegates);
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