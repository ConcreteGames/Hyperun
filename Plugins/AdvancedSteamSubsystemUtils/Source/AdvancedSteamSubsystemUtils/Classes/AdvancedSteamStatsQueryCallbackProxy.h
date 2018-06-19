// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"
#include "Engine.h"
#include "SteamStats.h"


#if !WITH_EDITOR
#include "Interfaces/AdvancedSteamStatsInterface.h"
#endif

#include "Interfaces/OnlineLeaderboardInterface.h"
#include "OnlineSubsystemUtilsClasses.h"

#include "AdvancedSteamStatsQueryCallbackProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSteamStatsQueryResult, EAdvancedSteamErrorCode, ErrorCode, const TArray<FSteamStats>&, OutStatsArray);

UCLASS(MinimalAPI)
class UAdvancedSteamStatsQueryCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// Called when there is a successful leaderboard query
	UPROPERTY(BlueprintAssignable)
	FSteamStatsQueryResult OnSuccess;

	// Called when there is an unsuccessful leaderboard query
	UPROPERTY(BlueprintAssignable)
	FSteamStatsQueryResult OnFailure;

	// Queries a leaderboard for an integer value
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", DisplayName="Read SteamStats"), Category="Online|SteamStats")
	static UAdvancedSteamStatsQueryCallbackProxy* CreateProxyObjectForIntQuery(TArray<FSteamStats> StatsArray);

	virtual void Activate() override;
public:
	//~ Begin UObject Interface
	virtual void BeginDestroy() override;
	//~ End UObject Interface

private:
	/** Called by the leaderboard system when the read is finished */
	void OnStatsRead(bool bWasSuccessful, const EAdvancedSteamErrorCode& InErrorCode);

	/** Unregisters our delegate from the leaderboard system */
	void RemoveDelegate();

	/** Triggers the query for a specifed user; the ReadObject must already be set up */
	void TriggerQuery(TArray<FSteamStats>& StatsArray);

private:
	EAdvancedSteamErrorCode ErrorCode;
#if !WITH_EDITOR

	IAdvancedSteamStatsInterfacePtr AdvancedSteamStatsInterface;
	/** Delegate called when a stats has been successfully read */
	FOnStatsReadCompleteDelegate StatsReadCompleteDelegate;

	/** StatsReadComplete delegate handle */
	FDelegateHandle StatsReadCompleteDelegateHandle;

#endif

	/** Stats request */
	TSharedPtr<class FSteamStatsRead, ESPMode::ThreadSafe> ReadObject;
	TArray<FSteamStats> StatsArray;

	/** Did we fail immediately? */
	bool bFailedToEvenSubmit;
};
