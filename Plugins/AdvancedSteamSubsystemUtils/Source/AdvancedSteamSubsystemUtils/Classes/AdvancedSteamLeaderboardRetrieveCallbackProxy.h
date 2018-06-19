// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Core.h"
#include "Engine.h"
#include "SteamLeaderboard.h"

#if !WITH_EDITOR
#include "Interfaces/AdvancedSteamLeaderboardInterface.h"
#include "Interfaces/AdvancedSteamStatsInterface.h"
#endif

#include "Interfaces/OnlineLeaderboardInterface.h"
#include "OnlineSubsystemUtilsClasses.h"

#include "AdvancedSteamLeaderboardRetrieveCallbackProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLeaderboardRetrieveResult, EAdvancedSteamErrorCode, ErrorCode, const TArray<FSteamLeaderboard>&, PlayerList);

UCLASS(MinimalAPI)
class UAdvancedSteamLeaderboardRetrieveCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// Called when there is a successful leaderboard Retrieve
	UPROPERTY(BlueprintAssignable)
	FLeaderboardRetrieveResult OnSuccess;

	// Called when there is an unsuccessful leaderboard Retrieve
	UPROPERTY(BlueprintAssignable)
	FLeaderboardRetrieveResult OnFailure;

	// Retrieve a leaderboard
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", DisplayName = "Retrieve SteamLeaderboard"), Category = "Online|SteamLeaderboard")
	static UAdvancedSteamLeaderboardRetrieveCallbackProxy* CreateProxyObjectForRetrieve(FName LeaboardName, FName StatName, TArray<FSteamStats> StatsArray, ELeaderboardRequest LeaderboardRequest, int32 RangeStart, int32 RangeEnd, int32 DetailCount);

	virtual void Activate() override;
public:
	//~ Begin UObject Interface
	virtual void BeginDestroy() override;
	//~ End UObject Interface

private:
	/** Called by the leaderboard system when the read is finished */
	void OnEntriesRead(bool InWasSuccessful, const EAdvancedSteamErrorCode& InErrorCode);
	void OnStatsRead(bool bWasSuccessful, const EAdvancedSteamErrorCode& InErrorCode, const FSteamStatsReadPtr SteamStatsReadPtr);

	/** Unregisters our delegate from the leaderboard system */
	void RemoveDelegate();
	void RemoveStatsDelegate();

private:
	FCriticalSection StatsLock;
	int32 RequestedStatsCount;
	TArray<FSteamStats> StatsArray;
#if !WITH_EDITOR

	IAdvancedSteamLeaderboardInterfacePtr AdvancedSteamLeaderboardInterface;
	/** Delegate called when a leaderboard has been successfully read */	
	FOnEntriesReadCompleteDelegate EntriesReadCompleteDelegate;
	/** StatsReadComplete delegate handle */
	FDelegateHandle EntriesReadCompleteDelegateHandle;


	IAdvancedSteamStatsInterfacePtr AdvancedSteamStatsInterface;
	/** Delegate called when a stats has been successfully read */
	FOnStatsReadRefCompleteDelegate StatsReadRefCompleteDelegate;

	/** StatsReadComplete delegate handle */
	FDelegateHandle StatsReadRefCompleteDelegateHandle;

#endif

	/** The leaderboard read request */
	TSharedPtr<class FSteamLeaderboardRead, ESPMode::ThreadSafe> ReadObject;

	/** Did we fail immediately? */
	bool bFailedToEvenSubmit;

	/** Name of the stat being queried */
	FName LeaboardName;
	FName StatName;
	ELeaderboardRequest LeaderboardRequest;
	int32 RangeStart;
	int32 RangeEnd;
	int32 DetailCount;

	// Did the read succeed?
	EAdvancedSteamErrorCode ErrorCode;
};
