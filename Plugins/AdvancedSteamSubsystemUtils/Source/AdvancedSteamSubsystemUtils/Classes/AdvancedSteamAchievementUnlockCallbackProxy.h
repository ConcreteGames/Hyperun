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

#include "AdvancedSteamAchievementUnlockCallbackProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSteamAchievementsUpdateResult, EAdvancedSteamErrorCode, ErrorCode);

UCLASS(MinimalAPI)
class UAdvancedSteamAchievementUnlockCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// Called when there is a successful leaderboard query
	UPROPERTY(BlueprintAssignable)
	FSteamAchievementsUpdateResult OnSuccess;

	// Called when there is an unsuccessful leaderboard query
	UPROPERTY(BlueprintAssignable)
	FSteamAchievementsUpdateResult OnFailure;

	// Queries a leaderboard for an integer value
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", DisplayName="Unlock SteamAchievement"), Category="Online|SteamAchievements")
	static UAdvancedSteamAchievementUnlockCallbackProxy* CreateProxyObjectForAchievementsUnlock(FName AchievementId);

	virtual void Activate() override;
public:
	//~ Begin UObject Interface
	virtual void BeginDestroy() override;
	//~ End UObject Interface

private:
	/** Called by the leaderboard system when the read is finished */
	void OnAchievementsUpdate(bool bWasSuccessful, const EAdvancedSteamErrorCode& InErrorCode);

	/** Unregisters our delegate from the leaderboard system */
	void RemoveDelegate();

	/** Triggers the query for a specifed user; the ReadObject must already be set up */
	void TriggerQuery(const FName& AchievementId);

private:
	EAdvancedSteamErrorCode ErrorCode;
	FName AchievementId;

#if !WITH_EDITOR

	IAdvancedSteamAchievementsInterfacePtr AdvancedSteamAchievementsInterface;
	/** Delegate called when a stats has been successfully read */
	FOnAchievementsUpdateCompleteDelegate AchievementsUpdateCompleteDelegate;

	/** AchievementsReadComplete delegate handle */
	FDelegateHandle AchievementsUpdateCompleteDelegateHandle;

	FUniqueNetIdSteam UserId;
#endif
	
	/** Did we fail immediately? */
	bool bFailedToEvenSubmit;
};
