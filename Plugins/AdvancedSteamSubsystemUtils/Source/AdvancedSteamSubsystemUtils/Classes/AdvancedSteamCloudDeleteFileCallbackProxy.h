// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"
#include "Engine.h"
#include "ErrorCodes.h"
#include "Online.h"

#include "Interfaces/OnlineSharedCloudInterface.h"
#include "OnlineSubsystemUtilsClasses.h"

#include "AdvancedSteamCloudDeleteFileCallbackProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCloudDeleteFileResult, FString, FilePath, EAdvancedSteamErrorCode, ErrorCode);

UCLASS(MinimalAPI)
class UAdvancedSteamCloudDeleteFileCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// Called when there is a successful leaderboard query
	UPROPERTY(BlueprintAssignable)
	FCloudDeleteFileResult OnSuccess;
	
	// Called when there is an unsuccessful leaderboard query
	UPROPERTY(BlueprintAssignable)
	FCloudDeleteFileResult OnFailure;

	// Queries a leaderboard for an integer value
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", DisplayName = "Delete SteamCloud File"), Category = "Online|SteamCloud")
	static UAdvancedSteamCloudDeleteFileCallbackProxy* CreateProxyObjectForDeleteCloud(FString FilePath);

	virtual void Activate() override;

private:
	void Init();
	
	void OnDeleteFile(bool InWasSuccessful, const EAdvancedSteamErrorCode& InErrorCode);

private:
	EAdvancedSteamErrorCode ErrorCode;

	FString FilePath;

	/** Convenient access to the cloud interfaces */
	IOnlineUserCloudPtr UserCloud;
	/** Delegates to various cloud functionality triggered */
	FOnDeleteUserFileCompleteDelegate OnDeleteUserFilesCompleteDelegate;
	FDelegateHandle OnDeleteUserFilesCompleteDelegateHandle;


	/** Did we fail immediately? */
	bool bWasSuccessful;

	/**
	* Delegate fired when a shared file write to the network platform's storage is complete
	*
	* @param bWasSuccessful whether the file Write was successful or not
	* @param UserId User owning the storage
	* @param FilePath the name of the file this was for
	* @param SharedHandle the handle to the shared file, may be platform dependent
	*/
	void OnDeleteUserFilesComplete(bool bWasSuccessful, const FUniqueNetId& InUserId, const FString& FileName);
};
