// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ErrorCodes.h"
#include "OnlineDelegateMacros.h"
#include "OnlineSubsystemTypes.h"
#include "OnlineStats.h"
#include "Private/OnlineSubsystemSteamTypes.h"

/**
* Delegate called when the last stats read is complete
*
* @param bWasSuccessful was the stats read successful or not
* @param ErrorCode was error code when it failed
*/

#if !WITH_EDITOR

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAttachFileComplete, bool, const EAdvancedSteamErrorCode&);
typedef FOnAttachFileComplete::FDelegate FOnAttachFileCompleteDelegate;

#endif

class IAdvancedSteamCloudInterface
{
protected:
	IAdvancedSteamCloudInterface() {}

public:
	virtual ~IAdvancedSteamCloudInterface() {}

	virtual bool WriteToCloud(const FName& LeaderboardName, const FName& StatName, const FString& UGCHandle) = 0;

#if !WITH_EDITOR
	/**
	* Notifies the interested party that the last stats read has completed
	*
	* @param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	DEFINE_ONLINE_DELEGATE_TWO_PARAM(OnAttachFileComplete, bool, const EAdvancedSteamErrorCode&);
#endif
	
};

typedef TSharedPtr<IAdvancedSteamCloudInterface, ESPMode::ThreadSafe> IAdvancedSteamCloudInterfacePtr;