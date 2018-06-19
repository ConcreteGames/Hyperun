// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#include "AdvancedSteamSubsystemUtilsPrivatePCH.h"
#include "AdvancedSteamCloudSharedWriteCallbackProxy.h"

#include "AdvancedSteam.h"
#include "AdvancedSteamCloudFile.h"
#include "OnlineSubsystemSteam.h"
#include "Private/OnlineSubsystemSteamTypes.h"

//////////////////////////////////////////////////////////////////////////
// ULeaderboardQueryCallbackProxy

UAdvancedSteamCloudSharedWriteCallbackProxy::UAdvancedSteamCloudSharedWriteCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Init();
}

void UAdvancedSteamCloudSharedWriteCallbackProxy::Activate()
{
#if WITH_EDITOR
	OnSuccess.Broadcast(TEXT("0"), ErrorCode);
#else
	bFailedToEvenSubmit = true;

	// compress files
	if (FAdvancedSteamCloudFile::SaveGameDataToFile(FileName, UploadFileList))
	{
		if (UploadReplay(FileName, UploadFileList))
		{
			bFailedToEvenSubmit = false;
		}
		else
		{
			OnAttachFile(false, ErrorCode);
		}
	}
	else
	{
		OnAttachFile(false, ErrorCode);
	}
#endif
}

void UAdvancedSteamCloudSharedWriteCallbackProxy::Init()
{
	ErrorCode = EAdvancedSteamErrorCode::UNKNOWN;

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);

	if (OnlineSub)
	{
		// Cache interfaces
		SharedCloud = OnlineSub->GetSharedCloudInterface();
		if (SharedCloud.IsValid())
		{
			// Setup delegates
			OnWriteSharedFileCompleteDelegate = FOnWriteSharedFileCompleteDelegate::CreateUObject(this, &UAdvancedSteamCloudSharedWriteCallbackProxy::OnWriteSharedFileComplete);
		}
		else
		{
			ErrorCode = EAdvancedSteamErrorCode::STEAM_NOT_INITIALIZED;
			UE_LOG(LogOnline, Warning, TEXT("SharedCloud test failed.  SharedCloud API not supported."));
		}
	}
#if !WITH_EDITOR
	UAdvancedSteam* AdvancedSteam = Cast<UAdvancedSteam>(UAdvancedSteam::StaticClass()->GetDefaultObject());
	if (AdvancedSteam)
	{
		AdvancedSteamCloudInterface = AdvancedSteam->GetAdvancedSteamCloudInterface();
	}
#endif
}

bool UAdvancedSteamCloudSharedWriteCallbackProxy::UploadReplay(const FName& InReplayName, TArray<FString>& FileList)
{
	if (SharedCloud.IsValid())
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
		if (OnlineSub && OnlineSub->GetIdentityInterface().IsValid())
		{
			OnWriteSharedFileCompleteDelegateHandle = SharedCloud->AddOnWriteSharedFileCompleteDelegate_Handle(OnWriteSharedFileCompleteDelegate);

			FString CompressedFileName = FString::Printf(TEXT("%s.zip"), *InReplayName.ToString());
			FString FullSavePath = FPaths::Combine(*(FPaths::GameSavedDir()), *FString::Printf(TEXT("%s"), *CompressedFileName));
			TArray<uint8> FileContents;
			if (FFileHelper::LoadFileToArray(FileContents, *FullSavePath))
			{
				TSharedPtr<const FUniqueNetId> UserId = OnlineSub->GetIdentityInterface()->GetUniquePlayerId(0);
				if (!SharedCloud->WriteSharedFile(*UserId.Get(), CompressedFileName, FileContents))
				{
					UE_LOG(LogOnline, Log, TEXT("UAdvancedSteamCloudSharedWriteCallbackProxy::UploadReplay failed to write SharedFile"));
					return false;
				}
			}
			else
			{
				ErrorCode = EAdvancedSteamErrorCode::FILE_NOT_FOUND;
				UE_LOG(LogOnline, Log, TEXT("UAdvancedSteamCloudSharedWriteCallbackProxy::UploadReplay failed to load file data"));
				return false;
			}
			return true;
		}
	}
	else
	{
		ErrorCode = EAdvancedSteamErrorCode::STEAM_NOT_INITIALIZED;
	}

	return false;
}


void UAdvancedSteamCloudSharedWriteCallbackProxy::OnWriteSharedFileComplete(bool bInWasSuccessful, const FUniqueNetId& InUserId, const FString& InFileName, const TSharedRef<FSharedContentHandle>& ContentHandle)
{
#if !WITH_EDITOR
	UE_LOG(LogOnline, Log, TEXT("Shared user file complete Success:%d UserId:%s FileName:%s"), bWasSuccessful, *InUserId.ToDebugString(), *InFileName);
	UE_LOG(LogOnline, Log, TEXT("------------------------------------------------"));

	// completed to upload UGC file
	SharedCloud->ClearOnWriteSharedFileCompleteDelegate_Handle(OnWriteSharedFileCompleteDelegateHandle);

	if (bInWasSuccessful)
	{
		// upload to steam
		if (AdvancedSteamCloudInterface.IsValid())
		{
			AttachFileCompleteDelegate = FOnAttachFileCompleteDelegate::CreateUObject(this, &UAdvancedSteamCloudSharedWriteCallbackProxy::OnAttachFile);
			AttachFileCompleteDelegateHandle = AdvancedSteamCloudInterface->AddOnAttachFileCompleteDelegate_Handle(AttachFileCompleteDelegate);
			bFailedToEvenSubmit = false;
			UGCHandle = ContentHandle->ToString();

			if (AdvancedSteamCloudInterface->WriteToCloud(LeaderboardName, StatName, ContentHandle->ToString()))
			{
				FFrame::KismetExecutionMessage(TEXT("ULeaderboardRetrieveCallbackProxy::AttachFileToLeaderboard - called AttachFileToLeaderboard"), ELogVerbosity::Warning);
			}
			else
			{
				ErrorCode = EAdvancedSteamErrorCode::STEAM_NOT_INITIALIZED;
				FFrame::KismetExecutionMessage(TEXT("ULeaderboardRetrieveCallbackProxy::AttachFileToLeaderboard - failed call AttachFileToLeaderboard"), ELogVerbosity::Warning);
				OnAttachFile(false, ErrorCode);
			}

		}
		else
		{
			ErrorCode = EAdvancedSteamErrorCode::STEAM_NOT_INITIALIZED;
			OnAttachFile(false, ErrorCode);
		}
	}
	else
	{
		// upload leaderboard without replay
		OnAttachFile(false, ErrorCode);
	}
#endif
}

void UAdvancedSteamCloudSharedWriteCallbackProxy::OnAttachFile(bool InWasSuccessful, const EAdvancedSteamErrorCode& InErrorCode)
{
	RemoveDelegate();

	bWasSuccessful = InWasSuccessful;
	if (SharedCloud.IsValid())
	{
		SharedCloud->ClearOnWriteSharedFileCompleteDelegate_Handle(OnWriteSharedFileCompleteDelegateHandle);
	}

	if (bWasSuccessful)
	{
		OnSuccess.Broadcast(UGCHandle, ErrorCode);
	}
	else
	{
		OnFailure.Broadcast(UGCHandle, ErrorCode);
	}
}

void UAdvancedSteamCloudSharedWriteCallbackProxy::RemoveDelegate()
{
#if !WITH_EDITOR
	if (AdvancedSteamCloudInterface.IsValid() && !bFailedToEvenSubmit)
	{
		AdvancedSteamCloudInterface->ClearOnAttachFileCompleteDelegate_Handle(AttachFileCompleteDelegateHandle);
	}
#endif
}

void UAdvancedSteamCloudSharedWriteCallbackProxy::BeginDestroy()
{
	RemoveDelegate();

	Super::BeginDestroy();
}

UAdvancedSteamCloudSharedWriteCallbackProxy* UAdvancedSteamCloudSharedWriteCallbackProxy::CreateProxyObjectForWriteCloud(FName LeaderboardName, FName StatName, FName FileName, TArray <FString> UploadFileList)
{
	UAdvancedSteamCloudSharedWriteCallbackProxy* Proxy = NewObject<UAdvancedSteamCloudSharedWriteCallbackProxy>();
	Proxy->SetFlags(RF_StrongRefOnFrame);
	Proxy->LeaderboardName = LeaderboardName;
	Proxy->StatName = StatName;
	Proxy->FileName = FileName;
	Proxy->UploadFileList = UploadFileList;

	return Proxy;
}
