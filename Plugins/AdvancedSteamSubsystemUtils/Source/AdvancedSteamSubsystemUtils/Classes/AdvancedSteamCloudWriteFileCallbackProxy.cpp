// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#include "AdvancedSteamSubsystemUtilsPrivatePCH.h"
#include "AdvancedSteamCloudWriteFileCallbackProxy.h"

#include "AdvancedSteam.h"
#include "AdvancedSteamCloudFile.h"
#include "OnlineSubsystemSteam.h"
#include "Private/OnlineSubsystemSteamTypes.h"

//////////////////////////////////////////////////////////////////////////
// ULeaderboardQueryCallbackProxy

UAdvancedSteamCloudWriteFileCallbackProxy::UAdvancedSteamCloudWriteFileCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Init();
}

void UAdvancedSteamCloudWriteFileCallbackProxy::Activate()
{
	if (UserCloud.IsValid())
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
		if (OnlineSub && OnlineSub->GetIdentityInterface().IsValid())
		{
			//FString FileName = FPaths::GetCleanFilename(FilePath);
			OnWriteUserCloudFileCompleteDelegateHandle = UserCloud->AddOnWriteUserFileCompleteDelegate_Handle(OnWriteUserCloudFileCompleteDelegate);
			TArray<uint8> FileContents;
			FString FullSavePath = FPaths::Combine(*(FPaths::GameSavedDir()), *FString::Printf(TEXT("%s"), *FilePath));
			if (FFileHelper::LoadFileToArray(FileContents, *FullSavePath))
			{
				TSharedPtr<const FUniqueNetId> UserId = OnlineSub->GetIdentityInterface()->GetUniquePlayerId(0);
				if (!UserCloud->WriteUserFile(*UserId.Get(), FilePath, FileContents))
				{
					ErrorCode = EAdvancedSteamErrorCode::FAILED_TO_WRITE_FILE;
					UE_LOG(LogOnline, Log, TEXT("UAdvancedSteamCloudWriteFileCallbackProxy::Activate failed to write File"));
					OnWriteFile(false, ErrorCode);
				}
			}
			else
			{
				ErrorCode = EAdvancedSteamErrorCode::FILE_NOT_FOUND;
				UE_LOG(LogOnline, Log, TEXT("UAdvancedSteamCloudWriteFileCallbackProxy::Activate failed to load file data"));
				OnWriteFile(false, ErrorCode);
			}
		}
	}
	else
	{
		ErrorCode = EAdvancedSteamErrorCode::STEAM_NOT_INITIALIZED;
		OnWriteFile(false, ErrorCode);
	}
}

void UAdvancedSteamCloudWriteFileCallbackProxy::Init()
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
			OnWriteUserCloudFileCompleteDelegate = FOnWriteUserFileCompleteDelegate::CreateUObject(this, &UAdvancedSteamCloudWriteFileCallbackProxy::OnWriteUserCloudFileComplete);
		}
		else
		{
			ErrorCode = EAdvancedSteamErrorCode::STEAM_NOT_INITIALIZED;
			UE_LOG(LogOnline, Warning, TEXT("UserCloud test failed.  UserCloud API not supported."));
		}
	}
}

void UAdvancedSteamCloudWriteFileCallbackProxy::OnWriteUserCloudFileComplete(bool bInWasSuccessful, const FUniqueNetId& InUserId, const FString& FileName)
{
	UE_LOG(LogOnline, Log, TEXT("Write user file complete Success:%d UserId:%s FileName:%s"), bWasSuccessful, *InUserId.ToDebugString(), *FileName);
	UE_LOG(LogOnline, Log, TEXT("------------------------------------------------"));

	// completed to upload UGC file
	UserCloud->ClearOnWriteUserFileCompleteDelegate_Handle(OnWriteUserCloudFileCompleteDelegateHandle);

	if (bInWasSuccessful)
	{
		OnWriteFile(true, ErrorCode);
	}
	else
	{
		// upload leaderboard without replay
		OnWriteFile(false, ErrorCode);
	}
}

void UAdvancedSteamCloudWriteFileCallbackProxy::OnWriteFile(bool InWasSuccessful, const EAdvancedSteamErrorCode& InErrorCode)
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

UAdvancedSteamCloudWriteFileCallbackProxy* UAdvancedSteamCloudWriteFileCallbackProxy::CreateProxyObjectForWriteCloud(FString FilePath)
{
	UAdvancedSteamCloudWriteFileCallbackProxy* Proxy = NewObject<UAdvancedSteamCloudWriteFileCallbackProxy>();
	Proxy->SetFlags(RF_StrongRefOnFrame);
	Proxy->FilePath = FilePath;

	return Proxy;
}
