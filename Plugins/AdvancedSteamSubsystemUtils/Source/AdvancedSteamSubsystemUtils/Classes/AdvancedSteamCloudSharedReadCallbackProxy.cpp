// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#include "AdvancedSteamSubsystemUtilsPrivatePCH.h"
#include "AdvancedSteamCloudSharedReadCallbackProxy.h"

#include "AdvancedSteam.h"
#include "AdvancedSteamCloudFile.h"
#include "OnlineSubsystemSteam.h"
#include "Private/OnlineSubsystemSteamTypes.h"
#include "Private/OnlineSharedCloudInterfaceSteam.h"

//////////////////////////////////////////////////////////////////////////
// ULeaderboardQueryCallbackProxy

UAdvancedSteamCloudSharedReadCallbackProxy::UAdvancedSteamCloudSharedReadCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Init();
}

void UAdvancedSteamCloudSharedReadCallbackProxy::Activate()
{
#if WITH_EDITOR
	OnSuccess.Broadcast(TEXT("0"), ErrorCode);
#else
	bFailedToEvenSubmit = true;

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
	if (OnlineSub && OnlineSub->GetIdentityInterface().IsValid())
	{
		if (SharedCloud.IsValid())
		{
			bFailedToEvenSubmit = false;
			TSharedRef<FSharedContentHandle> SharedHandle = MakeShareable(new FSharedContentHandleSteam(FCString::Atoi64(*UGCHandle)));
			OnReadSharedFileCompleteDelegateHandle = SharedCloud->AddOnReadSharedFileCompleteDelegate_Handle(OnReadSharedFileCompleteDelegate);
			if (!SharedCloud->ReadSharedFile(SharedHandle.Get()))
			{
				ErrorCode = EAdvancedSteamErrorCode::STEAM_NOT_INITIALIZED;
				UE_LOG(LogOnline, Log, TEXT("UAdvancedSteamCloudSharedReadCallbackProxy::UploadReplay failed to write SharedFile"));
				OnAttachFile(false, ErrorCode);
			}
		}
		else
		{
			ErrorCode = EAdvancedSteamErrorCode::STEAM_NOT_INITIALIZED;
			UE_LOG(LogOnline, Log, TEXT("UAdvancedSteamCloudSharedReadCallbackProxy::UploadReplay failed to load file data"));
			OnAttachFile(false, ErrorCode);
		}
	}
	else
	{
		ErrorCode = EAdvancedSteamErrorCode::STEAM_NOT_INITIALIZED;
		OnAttachFile(false, ErrorCode);
	}
#endif
}

void UAdvancedSteamCloudSharedReadCallbackProxy::Init()
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
			OnReadSharedFileCompleteDelegate = FOnReadSharedFileCompleteDelegate::CreateUObject(this, &UAdvancedSteamCloudSharedReadCallbackProxy::OnReadSharedFileComplete);
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

void UAdvancedSteamCloudSharedReadCallbackProxy::OnReadSharedFileComplete(bool bInWasSuccessful, const FSharedContentHandle& ContentHandle)
{
#if !WITH_EDITOR
	UE_LOG(LogOnline, Log, TEXT("Shared user file complete Success:%d ContentHandle:%s"), bWasSuccessful, *ContentHandle.ToString());
	UE_LOG(LogOnline, Log, TEXT("------------------------------------------------"));

	// completed to upload UGC file
	if (SharedCloud.IsValid())
	{
		SharedCloud->ClearOnReadSharedFileCompleteDelegate_Handle(OnReadSharedFileCompleteDelegateHandle);
	}

	if (bInWasSuccessful)
	{
		if (SharedCloud.IsValid())
		{
			FOnlineSharedCloudSteamPtr SteamSharedCloud = StaticCastSharedPtr<FOnlineSharedCloudSteam>(SharedCloud);
			if (SteamSharedCloud.IsValid())
			{
				FCloudFileSteam* SharedFile = SteamSharedCloud->GetSharedCloudFile(ContentHandle);
				if (SharedFile)
				{
					// decompress downloaded file to demo path
					if (FAdvancedSteamCloudFile::SaveReplayFile(SharedFile->Data))
					{
						OnAttachFile(true, ErrorCode);
					}
					else
					{
						ErrorCode = EAdvancedSteamErrorCode::FAILED_TO_SAVE_REPLAY;
						OnAttachFile(false, ErrorCode);
					}
				}
			}
		}
	}
	else
	{
		OnAttachFile(false, ErrorCode);
	}
#endif
}

void UAdvancedSteamCloudSharedReadCallbackProxy::OnAttachFile(bool InWasSuccessful, const EAdvancedSteamErrorCode& InErrorCode)
{
	RemoveDelegate();

	bWasSuccessful = InWasSuccessful;
	if (SharedCloud.IsValid())
	{
		SharedCloud->ClearOnReadSharedFileCompleteDelegate_Handle(OnReadSharedFileCompleteDelegateHandle);
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

void UAdvancedSteamCloudSharedReadCallbackProxy::RemoveDelegate()
{
#if !WITH_EDITOR
	if (AdvancedSteamCloudInterface.IsValid() && !bFailedToEvenSubmit)
	{
		AdvancedSteamCloudInterface->ClearOnAttachFileCompleteDelegate_Handle(AttachFileCompleteDelegateHandle);
	}
#endif
}

void UAdvancedSteamCloudSharedReadCallbackProxy::BeginDestroy()
{
	RemoveDelegate();

	Super::BeginDestroy();
}

UAdvancedSteamCloudSharedReadCallbackProxy* UAdvancedSteamCloudSharedReadCallbackProxy::CreateProxyObjectForReadCloud(FString UGCHandle)
{
	UAdvancedSteamCloudSharedReadCallbackProxy* Proxy = NewObject<UAdvancedSteamCloudSharedReadCallbackProxy>();
	Proxy->SetFlags(RF_StrongRefOnFrame);
	Proxy->UGCHandle = UGCHandle;

	return Proxy;
}
