// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ErrorCodes.h"
#include "OnlineSubsystemTypes.h"
#include "Private/OnlineSubsystemSteamTypes.h"
#include "steam/isteamremotestorage.h"
#include "SteamStats.h"

#include "SteamLeaderboard.generated.h"

UENUM(BlueprintType)
enum ELeaderboardRequest
{
	ELeaderboardRequestGlobal = 0		UMETA(DisplayName = "Global"), 				// requests rows in the leaderboard from the full table, with RangeStart & RangeEnd in the range [1, TotalEntries]
	ELeaderboardRequestGlobalAroundUser UMETA(DisplayName = "GlobalAroundUser"),	// requests rows around the current user, RangeStart being negate. E.g. when Range Start is 3 and Range End is 3, it will return 7 rows, 3 before the user, 3 after
	ELeaderboardRequestFriends			UMETA(DisplayName = "Friends"),				// requests all the rows for friends of the current user
	ELeaderboardRequestUsers			UMETA(DisplayName = "Users")				// downloads leaderboard entries for an arbitrary set of users. if a user doesn't have a leaderboard entry, they won't be included in the result a max of 100 users can be downloaded at a time, with only one outstanding call at a time
};

UENUM(BlueprintType)
enum ELeaderboardUploadMethod
{
	ELeaderboardUploadMethodKeepBest = 0	UMETA(DisplayName = "KeepBest"),	// Leaderboard will keep user's best score
	ELeaderboardUploadMethodForceUpdate		UMETA(DisplayName = "ForceUpdate")	// Leaderboard will always replace score with specified
};

USTRUCT(BlueprintType)
struct FSteamLeaderboard
{
	GENERATED_USTRUCT_BODY()
public:
	FSteamLeaderboard() : PlayerName() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SteamLeaderboard)
	FName PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SteamLeaderboard)
	int32 Score;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SteamLeaderboard)
	int32 Rank;

	// user with the entry - use SteamFriends()->GetFriendPersonaName() & SteamFriends()->GetFriendAvatar() to get more info
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SteamLeaderboard)
	FName PlayerId;

	/** All requested data on the leaderboard for this player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SteamLeaderboard)
	TArray<FSteamStats> Columns;

	/** Details are extra game-defined information regarding how the user got that score */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SteamLeaderboard)
	TArray<int32> Details;

	/** Avatar icon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SteamLeaderboard)
	int32 IconImage;

	// handle for UGC attached to the entry. Blueprint doesn't support int64
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SteamLeaderboard)
	FString HandleUGC;

	FSteamLeaderboard(const FName& InPlayerName,
		const FName& InPlayerId,
		const int32 InScore = 0, 
		const int32 InRank = 0, 
		const TArray<int32>& InDetails = TArray<int32>(),
		const int32 InIconImage = 0
		)
		: PlayerName(InPlayerName),
		PlayerId(InPlayerId), 
		Score(InScore), 
		Rank(InRank), 
		Details(InDetails),
		IconImage(InIconImage)
	{
	}
};

/**
* Basic leaderboard representation
*/
class FLeaderboardMetadata
{
	FLeaderboardMetadata() {}

public:

	/** Name of leaderboard, matches Steam backend */
	FName LeaderboardName;
	/** Sort Method */
	ELeaderboardSort::Type SortMethod;
	/** Display Type */
	ELeaderboardFormat::Type DisplayFormat;
	/** Number of entries on leaderboard */
	int32 TotalLeaderboardRows;
	/** Handle to leaderboard */
	SteamLeaderboard_t LeaderboardHandle;
	/** State of the leaderboard handle download */
	EOnlineAsyncTaskState::Type AsyncState;

	FLeaderboardMetadata(const FName& InLeaderboardName, ELeaderboardSort::Type InSortMethod, ELeaderboardFormat::Type InDisplayFormat) :
		LeaderboardName(InLeaderboardName),
		SortMethod(InSortMethod),
		DisplayFormat(InDisplayFormat),
		TotalLeaderboardRows(0),
		LeaderboardHandle(-1),
		AsyncState(EOnlineAsyncTaskState::NotStarted)
	{
	}

	FLeaderboardMetadata(const FName& InLeaderboardName) :
		LeaderboardName(InLeaderboardName),
		SortMethod(ELeaderboardSort::None),
		DisplayFormat(ELeaderboardFormat::Number),
		TotalLeaderboardRows(0),
		LeaderboardHandle(-1),
		AsyncState(EOnlineAsyncTaskState::NotStarted)
	{
	}
};


/**
*	Interface for reading data from a leaderboard service
*/
class FSteamLeaderboardRead
{
public:
	/** Name of the leaderboard read */
	FName LeaderboardName;
	/** Column this leaderboard is sorted by */
	FName SortedColumn;
	/** Column metadata for this leaderboard */
	TArray<FLeaderboardMetadata> ColumnMetadata;
	/** Array of ranked users retrieved (not necessarily sorted yet) */
	TArray<FSteamLeaderboard> Rows;
	/** Indicates an error reading data occurred while processing */
	EOnlineAsyncTaskState::Type ReadState;

	FSteamLeaderboardRead() :
		LeaderboardName(NAME_None),
		SortedColumn(NAME_None),
		ReadState(EOnlineAsyncTaskState::NotStarted)
	{
	}

	/**
	*	Retrieve a single record from the leaderboard for a given user
	*
	* @param UserId user id to retrieve a record for
	* @return the requested user row or nullptr if not found
	*/
	FSteamLeaderboard* FindPlayerRecord(const FName& PlayerId)
	{
		for (int32 UserIdx = 0; UserIdx<Rows.Num(); UserIdx++)
		{
			if (Rows[UserIdx].PlayerId == PlayerId)
			{
				return &Rows[UserIdx];
			}
		}

		return nullptr;
	}
};

typedef TSharedRef<FSteamLeaderboardRead, ESPMode::ThreadSafe> FSteamLeaderboardReadRef;
typedef TSharedPtr<FSteamLeaderboardRead, ESPMode::ThreadSafe> FSteamLeaderboardReadPtr;