// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#include "AdvancedSteamSubsystemUtilsPrivatePCH.h"
#include "AdvancedSteamCloudDeleteFileCallbackProxy.h"

#include "AdvancedSteam.h"
#include "AdvancedSteamCloudFile.h"
#include "OnlineSubsystemSteam.h"
#include "Private/OnlineSubsystemSteamTypes.h"

//////////////////////////////////////////////////////////////////////////
// ULeaderboardQueryCallbackProxy

UAdvancedSteamCloudDeleteFileCallbackProxy::UAdvancedSteamCloudDeleteFileCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Init();
}

void UAdvancedSteamCloudDeleteFileCallbackProxy::Activate()
{
	if (UserCloud.IsValid())
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
		if (OnlineSub && OnlineSub->GetIdentityInterface().IsValid())
		{
			//FString FileName = FPaths::GetCleanFilename(FilePath);
			OnDeleteUserFilesCompleteDelegateHandle = UserCloud->AddOnDeleteUserFileCompleteDelegate_Handle(OnDeleteUserFilesCompleteDelegate);
			TSharedPtr<const FUniqueNetId> UserId = OnlineSub->GetIdentityInterface()->GetUniquePlayerId(0);
			if (!UserCloud->DeleteUserFile(*UserId, FilePath, true, true))
			{
				ErrorCode = EAdvancedSteamErrorCode::FAILED_TO_DELETE_FILE;
				UE_LOG(LogOnline, Log, TEXT("UAdvancedSteamCloudDeleteFileCallbackProxy::Activate failed to delete cloud file"));
				OnDeleteFile(false, ErrorCode);
			}
		}
	}
	else
	{
		ErrorCode = EAdvancedSteamErrorCode::STEAM_NOT_INITIALIZED;
		OnDeleteFile(false, ErrorCode);
	}
}

void UAdvancedSteamCloudDeleteFileCallbackProxy::Init()
{
	ErrorCode = EAdvancedSteamErrorCode::UNKNOWN;

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);

	if (OnlineSub)
	{
		// Cache interfaces
		UserCloud = OnlineSub->GetUserCloudInterface();
		if (UserCloud.IsValid())
		{
			// Setup delegates
			OnDeleteUserFilesCompleteDelegate = FOnDeleteUserFileCompleteDelegate::CreateUObject(this, &UAdvancedSteamCloudDeleteFileCallbackProxy::OnDeleteUserFilesComplete);
		}
		else
		{
			ErrorCode = EAdvancedSteamErrorCode::STEAM_NOT_INITIALIZED;
			UE_LOG(LogOnline, Warning, TEXT("UserCloud test failed.  UserCloud API not supported."));
		}
	}
}

void UAdvancedSteamCloudDeleteFileCallbackProxy::OnDeleteUserFilesComplete(bool bInWasSuccessful, const FUniqueNetId& InUserId, const FString& FileName)
{
	UE_LOG(LogOnline, Log, TEXT("Read user file complete Success:%d UserId:%s FileName:%s"), bWasSuccessful, *InUserId.ToDebugString(), *FileName);
	UE_LOG(LogOnline, Log, TEXT("------------------------------------------------"));

	// completed to upload UGC file
	UserCloud->ClearOnDeleteUserFileCompleteDelegate_Handle(OnDeleteUserFilesCompleteDelegateHandle);

	if (bInWasSuccessful)
	{
		OnDeleteFile(true, ErrorCode);
	}
	else
	{
		// upload leaderboard without replay
		OnDeleteFile(false, ErrorCode);
	}
}

void UAdvancedSteamCloudDeleteFileCallbackProxy::OnDeleteFile(bool InWasSuccessful, const EAdvancedSteamErrorCode& InErrorCode)
{
	bWasSuccessful = InWasSuccessful;

	if (bWasSuccessful)
	{
		OnSuccess.Broadcast(FilePath, ErrorCode);
	}
	else
	{
		OnFailure.Broadcast(FilePath, ErrorCode);
	}
}

UAdvancedSteamCloudDeleteFileCallbackProxy* UAdvancedSteamCloudDeleteFileCallbackProxy::CreateProxyObjectForDeleteCloud(FString FilePath)
{
	UAdvancedSteamCloudDeleteFileCallbackProxy* Proxy = NewObject<UAdvancedSteamCloudDeleteFileCallbackProxy>();
	Proxy->SetFlags(RF_StrongRefOnFrame);
	Proxy->FilePath = FilePath;

	return Proxy;
}
