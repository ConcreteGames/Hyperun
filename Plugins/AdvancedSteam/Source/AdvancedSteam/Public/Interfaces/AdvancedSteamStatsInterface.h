// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OnlineDelegateMacros.h"
#include "OnlineSubsystemTypes.h"
#include "Private/OnlineSubsystemSteamTypes.h"
#include "SteamStats.h"

/**
* Delegate called when the last stats read is complete
*
* @param bWasSuccessful was the stats read successful or not
* @param ErrorCode was error code when it failed
*/

#if !WITH_EDITOR
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatsReadComplete, bool, const EAdvancedSteamErrorCode&);
typedef FOnStatsReadComplete::FDelegate FOnStatsReadCompleteDelegate;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnStatsReadRefComplete, bool, const EAdvancedSteamErrorCode&, const FSteamStatsReadPtr);
typedef FOnStatsReadRefComplete::FDelegate FOnStatsReadRefCompleteDelegate;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatsWriteComplete, bool, const EAdvancedSteamErrorCode&);
typedef FOnStatsWriteComplete::FDelegate FOnStatsWriteCompleteDelegate; 
#endif

class IAdvancedSteamStatsInterface
{
protected:
	IAdvancedSteamStatsInterface() {}

public:
	virtual ~IAdvancedSteamStatsInterface() {}

	virtual bool GetStats(const FUniqueNetIdSteam& InUserId, FSteamStatsReadPtr InReadObject, bool ShouldTriggerRefDelegates) = 0;
	virtual bool SetStats(const FUniqueNetIdSteam& InUserId, FSteamStatsReadPtr InReadObject, bool InIncreaseStat) = 0;

	/**
	* Notifies the interested party that the last stats read has completed
	*
	* @param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
#if !WITH_EDITOR
	DEFINE_ONLINE_DELEGATE_TWO_PARAM(OnStatsReadComplete, bool, const EAdvancedSteamErrorCode&);
	DEFINE_ONLINE_DELEGATE_THREE_PARAM(OnStatsReadRefComplete, bool, const EAdvancedSteamErrorCode&, const FSteamStatsReadPtr);
	DEFINE_ONLINE_DELEGATE_TWO_PARAM(OnStatsWriteComplete, bool, const EAdvancedSteamErrorCode&);
#endif

};

typedef TSharedPtr<IAdvancedSteamStatsInterface, ESPMode::ThreadSafe> IAdvancedSteamStatsInterfacePtr;