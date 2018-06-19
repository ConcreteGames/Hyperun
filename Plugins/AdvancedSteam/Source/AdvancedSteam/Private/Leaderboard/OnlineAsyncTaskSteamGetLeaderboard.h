// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once
/**
*	Async task to retrieve all leaderboards for a single user from the Steam backend
*/

#include "Interfaces/AdvancedSteamLeaderboardInterface.h"
#include "Interfaces/OnlineLeaderboardInterface.h"
#include "Private/OnlineSubsystemSteamTypes.h"

#include "Private/OnlineAsyncTaskManagerSteam.h"
#include "SteamStats.h"


class FOnlineAsyncTaskSteam;
class FOnlineSubsystemSteam;
class FAdvancedSteamLeaderboard;

class FOnlineAsyncTaskSteamGetLeaderboard : public FOnlineAsyncTaskSteam
{
private:

	/** Has this request been started */
	bool bInit;
	/** Name of requested leaderboard */
	FName LeaderboardName;
	/** Method of sorting the scores on the leaderboard */
	ELeaderboardSort::Type SortMethod;
	/** Method of displaying the data on the leaderboard */
	ELeaderboardFormat::Type DisplayFormat;
	/** Results returned from Steam backend */
	LeaderboardFindResult_t CallbackResults;
	/** Should find only */
	bool bFindOnly;

	/** Potentially multiple user requests involved in filling in the read object, should this one trigger the finished delegate */
	FAdvancedSteamLeaderboard* AdvancedSteamLeaderboardPtr;

	/**
	*	Actually create and find a leaderboard with the Steam backend
	* If the leaderboard already exists, the leaderboard data will still be retrieved
	* @param LeaderboardName name of leaderboard to create
	* @param SortMethod method the leaderboard scores will be sorted, ignored if leaderboard exists
	* @param DisplayFormat type of data the leaderboard represents, ignored if leaderboard exists
	*/
	void CreateOrFindLeaderboard(const FName& InLeaderboardName, ELeaderboardSort::Type InSortMethod, ELeaderboardFormat::Type InDisplayFormat);

	/** Hidden on purpose */
	FOnlineAsyncTaskSteamGetLeaderboard();

public:

	/** Create a leaderboard implementation */
	FOnlineAsyncTaskSteamGetLeaderboard(
		FOnlineSubsystemSteam* InSteamSubsystem, 
		const FName& InLeaderboardName, 
		ELeaderboardSort::Type InSortMethod, 
		ELeaderboardFormat::Type InDisplayFormat, 
		FAdvancedSteamLeaderboard* InAdvancedSteamLeaderboard);

	/** Find a leaderboard implementation */
	FOnlineAsyncTaskSteamGetLeaderboard(
		FOnlineSubsystemSteam* InSteamSubsystem, 
		const FName& InLeaderboardName, 
		FAdvancedSteamLeaderboard* InAdvancedSteamLeaderboard);

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
};