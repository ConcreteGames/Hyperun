// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"
#include "Engine.h"
#include "SteamAchievements.h"

#if !WITH_EDITOR
#include "Interfaces/AdvancedSteamAchievementsInterface.h"
#include "Private/OnlineSubsystemSteamTypes.h"
#endif

#include "OnlineSubsystemUtilsClasses.h"

#include "AdvancedSteamAchievementQueryCallbackProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSteamAchievementsQueryResult, EAdvancedSteamErrorCode, ErrorCode, const TArray<FSteamAchievements>&, OutAchievementsArray);

UCLASS(MinimalAPI)
class UAdvancedSteamAchievementQueryCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// Called when there is a successful leaderboard query
	UPROPERTY(BlueprintAssignable)
	FSteamAchievementsQueryResult OnSuccess;

	// Called when there is an unsuccessful leaderboard query
	UPROPERTY(BlueprintAssignable)
	FSteamAchievementsQueryResult OnFailure;

	// Queries a leaderboard for an integer value
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", DisplayName="Read SteamAchievements"), Category="Online|SteamAchievements")
	static UAdvancedSteamAchievementQueryCallbackProxy* CreateProxyObjectForAchievementsQuery();

	virtual void Activate() override;
public:
	//~ Begin UObject Interface
	virtual void BeginDestroy() override;
	//~ End UObject Interface

private:
	/** Called by the leaderboard system when the read is finished */
	void OnAchievementsRead(bool bWasSuccessful, const EAdvancedSteamErrorCode& InErrorCode);

	/** Unregisters our delegate from the leaderboard system */
	void RemoveDelegate();

private:
	EAdvancedSteamErrorCode ErrorCode;
#if !WITH_EDITOR

	IAdvancedSteamAchievementsInterfacePtr AdvancedSteamAchievementsInterface;
	/** Delegate called when a stats has been successfully read */
	FOnAchievementsReadCompleteDelegate AchievementsReadCompleteDelegate;

	/** AchievementsReadComplete delegate handle */
	FDelegateHandle AchievementsReadCompleteDelegateHandle;

	FUniqueNetIdSteam UserId;
#endif
	
	/** Did we fail immediately? */
	bool bFailedToEvenSubmit;
};
