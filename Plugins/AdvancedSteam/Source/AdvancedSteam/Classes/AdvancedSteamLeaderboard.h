// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Interfaces/AdvancedSteamLeaderboardInterface.h"

class ADVANCEDSTEAM_API FAdvancedSteamLeaderboard : public IAdvancedSteamLeaderboardInterface
{
public:
	FAdvancedSteamLeaderboard();
	virtual ~FAdvancedSteamLeaderboard();

	virtual bool DownloadLeaderboardEntries(const TArray< TSharedRef<const FUniqueNetId> >& InPlayers, const FSteamLeaderboardReadRef& InReadObject, ELeaderboardRequest LeaderboardDataRequest, int32 RangeStart, int32 RangeEnd, int32 DetailCount) override;
	virtual bool UploadLeaderboardScore(const ELeaderboardUploadMethod LeaderboardUploadMethod, const FName& LeaderboardName, const FName& StatName, int32 StatValue, const TArray<int32>& InDetails) override;
	virtual UTexture2D* GetAvatarIcon(const int32 Image) override;

#if !WITH_EDITOR
	virtual FLeaderboardMetadata* GetLeaderboardMetadata(const FName& LeaderboardName, bool CreateIfNotExist=false) override;

protected:
	/** Critical section for thread safe operation of the leaderboard metadata */
	FCriticalSection LeaderboardMetadataLock;

	/** Array of known leaderboards (may be more that haven't been requested from) */
	TArray<FLeaderboardMetadata> Leaderboards;

	/**
	*	Start an async task to create a leaderboard with the Steam backend
	* If the leaderboard already exists, the leaderboard data will still be retrieved
	* @param LeaderboardName name of leaderboard to create
	* @param SortMethod method the leaderboard scores will be sorted, ignored if leaderboard exists
	* @param DisplayFormat type of data the leaderboard represents, ignored if leaderboard exists
	*/
	FLeaderboardMetadata* CreateLeaderboard(const FName& LeaderboardName, ELeaderboardSort::Type SortMethod, ELeaderboardFormat::Type DisplayFormat);

	/**
	*	Start an async task to find a leaderboard with the Steam backend
	* If the leaderboard doesn't exist, a warning will be generated
	* @param LeaderboardName name of leaderboard to create
	*/
	void FindLeaderboard(const FName& LeaderboardName);
#endif

private:
	void Init();

	/** get player steam icon image */
	TMap<int, UTexture2D*> PlayerIconImageMap;
	UTexture2D* GetSteamImageAsTexture(int32 Image);
};

typedef TSharedPtr<FAdvancedSteamLeaderboard, ESPMode::ThreadSafe> FAdvancedSteamLeaderboardPtr;