// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"
#include "Engine.h"
#include "SteamStats.h"

#include "Interfaces/OnlineLeaderboardInterface.h"
#include "OnlineSubsystemUtilsClasses.h"

#include "AdvancedSteamLeaderboardQueryCallbackProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSteamLeaderboardQueryResult, int32, LeaderboardValue);

UCLASS(MinimalAPI)
class UAdvancedSteamLeaderboardQueryCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// Called when there is a successful leaderboard query
	UPROPERTY(BlueprintAssignable)
	FSteamLeaderboardQueryResult OnSuccess;

	// Called when there is an unsuccessful leaderboard query
	UPROPERTY(BlueprintAssignable)
	FSteamLeaderboardQueryResult OnFailure;

	// Queries a leaderboard for an integer value
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", DisplayName = "Read SteamLeaderboard Integer"), Category = "Online|SteamLeaderboard")
	static UAdvancedSteamLeaderboardQueryCallbackProxy* CreateProxyObjectForIntQuery(FName LeaboardName, FName StatName);

	virtual void Activate() override;
public:
	//~ Begin UObject Interface
	virtual void BeginDestroy() override;
	//~ End UObject Interface

private:
	/** Called by the leaderboard system when the read is finished */
	void OnStatsRead(bool bWasSuccessful);

	/** Unregisters our delegate from the leaderboard system */
	void RemoveDelegate();

private:
	/** Delegate called when a leaderboard has been successfully read */
	FOnLeaderboardReadCompleteDelegate LeaderboardReadCompleteDelegate;

	/** LeaderboardReadComplete delegate handle */
	FDelegateHandle LeaderboardReadCompleteDelegateHandle;

	/** The leaderboard read request */
	TSharedPtr<class FOnlineLeaderboardRead, ESPMode::ThreadSafe> ReadObject;

	/** Did we fail immediately? */
	bool bFailedToEvenSubmit;

	/** Name of the stat being queried */
	FName LeaboardName;
	FName StatName;
	
	// Did the read succeed?
	int32 SavedValue;
	FSteamStats StatsInfo;
	EAdvancedSteamErrorCode ErrorCode;
};
