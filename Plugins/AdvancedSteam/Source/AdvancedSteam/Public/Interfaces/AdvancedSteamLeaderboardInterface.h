// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OnlineDelegateMacros.h"
#include "OnlineSubsystemTypes.h"
#include "OnlineStats.h"
#include "Private/OnlineSubsystemSteamTypes.h"
#include "SteamLeaderboard.h"

/**
* Delegate called when the last stats read is complete
*
* @param bWasSuccessful was the stats read successful or not
* @param ErrorCode was error code when it failed
*/

#if !WITH_EDITOR
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEntriesReadComplete, bool, const EAdvancedSteamErrorCode&);
typedef FOnEntriesReadComplete::FDelegate FOnEntriesReadCompleteDelegate;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnScoreUploadComplete, bool, const EAdvancedSteamErrorCode&);
typedef FOnScoreUploadComplete::FDelegate FOnScoreUploadCompleteDelegate;

#endif

class IAdvancedSteamLeaderboardInterface
{
protected:
	IAdvancedSteamLeaderboardInterface() {}

public:
	virtual ~IAdvancedSteamLeaderboardInterface() {}

	virtual bool DownloadLeaderboardEntries(const TArray< TSharedRef<const FUniqueNetId> >& InPlayers, const FSteamLeaderboardReadRef& InReadObject, ELeaderboardRequest LeaderboardDataRequest, int32 RangeStart, int32 RangeEnd, int32 DetailCount) = 0;
	virtual bool UploadLeaderboardScore(const ELeaderboardUploadMethod LeaderboardUploadMethod, const FName& LeaderboardName, const FName& StatName, int32 StatValue, const TArray<int32>& InDetails) = 0;
	virtual UTexture2D* GetAvatarIcon(const int32 Image) = 0;
#if !WITH_EDITOR
	virtual FLeaderboardMetadata* GetLeaderboardMetadata(const FName& LeaderboardName, bool CreateIfNotExist = false) = 0;
	/**
	* Notifies the interested party that the last stats read has completed
	*
	* @param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	DEFINE_ONLINE_DELEGATE_TWO_PARAM(OnEntriesReadComplete, bool, const EAdvancedSteamErrorCode&);
	DEFINE_ONLINE_DELEGATE_TWO_PARAM(OnScoreUploadComplete, bool, const EAdvancedSteamErrorCode&);
#endif
	
};

typedef TSharedPtr<IAdvancedSteamLeaderboardInterface, ESPMode::ThreadSafe> IAdvancedSteamLeaderboardInterfacePtr;