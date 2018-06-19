// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"
#include "Engine.h"
#include "ErrorCodes.h"
#include "Online.h"

#if !WITH_EDITOR
#include "Interfaces/AdvancedSteamCloudInterface.h"
#endif

#include "Interfaces/OnlineSharedCloudInterface.h"
#include "OnlineSubsystemUtilsClasses.h"

#include "AdvancedSteamCloudSharedReadCallbackProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCloudReadResult, FString, UGCHandle, EAdvancedSteamErrorCode, ErrorCode);

UCLASS(MinimalAPI)
class UAdvancedSteamCloudSharedReadCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// Called when there is a successful leaderboard query
	UPROPERTY(BlueprintAssignable)
	FCloudReadResult OnSuccess;
	
	// Called when there is an unsuccessful leaderboard query
	UPROPERTY(BlueprintAssignable)
	FCloudReadResult OnFailure;

	// Queries a leaderboard for an integer value
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", DisplayName = "Read SteamCloud Shared File"), Category = "Online|SteamCloud")
	static UAdvancedSteamCloudSharedReadCallbackProxy* CreateProxyObjectForReadCloud(FString UGCHandle);

	virtual void Activate() override;
public:
	//~ Begin UObject Interface
	virtual void BeginDestroy() override;
	//~ End UObject Interface

private:
	void Init();
	
	void OnAttachFile(bool InWasSuccessful, const EAdvancedSteamErrorCode& InErrorCode);
	void RemoveDelegate();

private:

#if !WITH_EDITOR

	IAdvancedSteamCloudInterfacePtr AdvancedSteamCloudInterface;
	/** Delegate called when a leaderboard has been successfully read */
	FOnAttachFileCompleteDelegate AttachFileCompleteDelegate;
	/** StatsReadComplete delegate handle */
	FDelegateHandle AttachFileCompleteDelegateHandle;

#endif
	EAdvancedSteamErrorCode ErrorCode;

	FString UGCHandle;

	/** Did we fail immediately? */
	bool bFailedToEvenSubmit;
	bool bWasSuccessful;
	
	/** Convenient access to the cloud interfaces */
	IOnlineSharedCloudPtr SharedCloud;

	/** Delegates to various cloud functionality triggered */
	FOnReadSharedFileCompleteDelegate OnReadSharedFileCompleteDelegate;

	/** Handles to those delegates */
	FDelegateHandle OnReadSharedFileCompleteDelegateHandle;

	/**
	* Delegate fired when a shared file read from the network platform's storage is complete
	*
	* @param bWasSuccessful whether the file read was successful or not
	* @param SharedHandle the handle of the read content
	*/
	void OnReadSharedFileComplete(bool bInWasSuccessful, const FSharedContentHandle& ContentHandle);

	/** save replay file */
	bool SaveReplayFile(const TArray<uint8>& Data);
	bool VerifyOrCreateDirectory(const FString& Path);
};
