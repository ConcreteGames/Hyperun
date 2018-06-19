// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#include "AdvancedSteamSubsystemUtilsPrivatePCH.h"
#include "AdvancedSteamCloudReadFileCallbackProxy.h"

#include "AdvancedSteam.h"
#include "AdvancedSteamCloudFile.h"
#include "OnlineSubsystemSteam.h"
#include "Private/OnlineSubsystemSteamTypes.h"

//////////////////////////////////////////////////////////////////////////
// ULeaderboardQueryCallbackProxy

UAdvancedSteamCloudReadFileCallbackProxy::UAdvancedSteamCloudReadFileCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Init();
}

void UAdvancedSteamCloudReadFileCallbackProxy::Activate()
{
	if (UserCloud.IsValid())
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
		if (OnlineSub && OnlineSub->GetIdentityInterface().IsValid())
		{
			//FString FileName = FPaths::GetCleanFilename(FilePath);
			OnReadUserFileCompleteDelegateHandle = UserCloud->AddOnReadUserFileCompleteDelegate_Handle(OnReadUserFileCompleteDelegate);
			TSharedPtr<const FUniqueNetId> UserId = OnlineSub->GetIdentityInterface()->GetUniquePlayerId(0);
			if (!UserCloud->ReadUserFile(*UserId.Get(), FilePath))
			{
				ErrorCode = EAdvancedSteamErrorCode::FAILED_TO_READ_FILE;
				UE_LOG(LogOnline, Log, TEXT("UAdvancedSteamCloudReadFileCallbackProxy::Activate failed to read cloud file"));
				OnReadFile(false, ErrorCode);
			}
		}
	}
	else
	{
		ErrorCode = EAdvancedSteamErrorCode::STEAM_NOT_INITIALIZED;
		OnReadFile(false, ErrorCode);
	}
}

void UAdvancedSteamCloudReadFileCallbackProxy::Init()
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
			OnReadUserFileCompleteDelegate = FOnReadUserFileCompleteDelegate::CreateUObject(this, &UAdvancedSteamCloudReadFileCallbackProxy::OnReadUserFileComplete);
		}
		else
		{
			ErrorCode = EAdvancedSteamErrorCode::STEAM_NOT_INITIALIZED;
			UE_LOG(LogOnline, Warning, TEXT("UserCloud test failed.  UserCloud API not supported."));
		}
	}
}

void UAdvancedSteamCloudReadFileCallbackProxy::OnReadUserFileComplete(bool bInWasSuccessful, const FUniqueNetId& InUserId, const FString& FileName)
{
	UE_LOG(LogOnline, Log, TEXT("Read user file complete Success:%d UserId:%s FileName:%s"), bWasSuccessful, *InUserId.ToDebugString(), *FileName);
	UE_LOG(LogOnline, Log, TEXT("------------------------------------------------"));

	// completed to upload UGC file
	UserCloud->ClearOnReadUserFileCompleteDelegate_Handle(OnReadUserFileCompleteDelegateHandle);

	if (bInWasSuccessful)
	{
		// get cloud file binary data
		TArray<uint8> FileContents;
		bWasSuccessful = UserCloud->GetFileContents(InUserId, FileName, FileContents);

		FString FullSavePath = FPaths::Combine(*(FPaths::GameSavedDir()), *FString::Printf(TEXT("%s"), *FilePath));
		if (FFileHelper::SaveArrayToFile(FileContents, *FullSavePath))
		{
			OnReadFile(true, ErrorCode);
		}
		else
		{
			ErrorCode = EAdvancedSteamErrorCode::FAILED_TO_SAVE_FILE;
			OnReadFile(false, ErrorCode);
		}		
	}
	else
	{
		// upload leaderboard without replay
		OnReadFile(false, ErrorCode);
	}
}

void UAdvancedSteamCloudReadFileCallbackProxy::OnReadFile(bool InWasSuccessful, const EAdvancedSteamErrorCode& InErrorCode)
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

UAdvancedSteamCloudReadFileCallbackProxy* UAdvancedSteamCloudReadFileCallbackProxy::CreateProxyObjectForReadCloud(FString FilePath)
{
	UAdvancedSteamCloudReadFileCallbackProxy* Proxy = NewObject<UAdvancedSteamCloudReadFileCallbackProxy>();
	Proxy->SetFlags(RF_StrongRefOnFrame);
	Proxy->FilePath = FilePath;

	return Proxy;
}
