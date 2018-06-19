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

#include "AdvancedSteamCloudSharedWriteCallbackProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCloudWriteResult, FString, UGCHandle, EAdvancedSteamErrorCode, ErrorCode);

UCLASS(MinimalAPI)
class UAdvancedSteamCloudSharedWriteCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// Called when there is a successful leaderboard query
	UPROPERTY(BlueprintAssignable)
	FCloudWriteResult OnSuccess;
	
	// Called when there is an unsuccessful leaderboard query
	UPROPERTY(BlueprintAssignable)
	FCloudWriteResult OnFailure;

	// Queries a leaderboard for an integer value
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", DisplayName = "Write SteamCloud Shared File"), Category = "Online|SteamCloud")
	static UAdvancedSteamCloudSharedWriteCallbackProxy* CreateProxyObjectForWriteCloud(FName LeaderboardName, FName StatName, FName FileName, TArray <FString> UploadFileList);

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
	FName LeaderboardName;
	FName StatName;
	FName FileName;

	/** Did we fail immediately? */
	bool bFailedToEvenSubmit;
	bool bWasSuccessful;
	
	// Did the read succeed?
	bool UploadReplay(const FName& FileName, TArray<FString>& FileList);

	/** Convenient access to the cloud interfaces */
	IOnlineSharedCloudPtr SharedCloud;

	/** Demo file list */
	TArray <FString> UploadFileList;

	/** Delegates to various cloud functionality triggered */
	FOnWriteSharedFileCompleteDelegate OnWriteSharedFileCompleteDelegate;

	/** Handles to those delegates */
	FDelegateHandle OnWriteSharedFileCompleteDelegateHandle;

	/**
	* Delegate fired when a shared file write to the network platform's storage is complete
	*
	* @param bWasSuccessful whether the file Write was successful or not
	* @param UserId User owning the storage
	* @param Filename the name of the file this was for
	* @param SharedHandle the handle to the shared file, may be platform dependent
	*/
	void OnWriteSharedFileComplete(bool bWasSuccessful, const FUniqueNetId& UserId, const FString& FileName, const TSharedRef<FSharedContentHandle>& ContentHandle);
};
