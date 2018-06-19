// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AdvancedSteamSubsystemUtilsPrivatePCH.h"
#include "AdvancedSteamUICallbackProxy.h"

#include "OnlineSubsystemSteam.h"
#include "Private/OnlineSubsystemSteamTypes.h"

//////////////////////////////////////////////////////////////////////////
// UAdvancedSteamUICallbackProxy

UAdvancedSteamUICallbackProxy::UAdvancedSteamUICallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UAdvancedSteamUICallbackProxy::Activate()
{
#if WITH_EDITOR
	OnSuccess.Broadcast(ErrorCode);
#else
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub && OnlineSub->GetIdentityInterface().IsValid())
	{
		IOnlineExternalUIPtr OnlineExternalUI = OnlineSub->GetExternalUIInterface();
		if (OnlineExternalUI.IsValid())
		{
			// Define and register delegates
			bool ExternalUIOpened = OnlineExternalUI->ShowWebURL(
				WebURL,
				FShowWebUrlParams(),
				FOnShowWebUrlClosedDelegate());
			if (!ExternalUIOpened)
			{
				ErrorCode = EAdvancedSteamErrorCode::EXTERAL_WEBUI_OPEN_FAILED;
				FFrame::KismetExecutionMessage(TEXT("UAdvancedSteamUICallbackProxy::TriggerQuery - AdvancedSteamUI Invalid or Cannot map local player to unique net ID"), ELogVerbosity::Warning);
			}
			else
			{
				OnSuccess.Broadcast(ErrorCode);
				return;
			}
		}
		else
		{
			ErrorCode = EAdvancedSteamErrorCode::ADVANCED_STEAM_MODULE_NOT_INITIALIZED;
			FFrame::KismetExecutionMessage(TEXT("UAdvancedSteamUICallbackProxy::TriggerQuery - AdvancedSteamUI Invalid or Cannot map local player to unique net ID"), ELogVerbosity::Warning);
		}
	}
	else
	{
		ErrorCode = EAdvancedSteamErrorCode::STEAM_NOT_INITIALIZED;
		FFrame::KismetExecutionMessage(TEXT("UAdvancedSteamUICallbackProxy::TriggerQuery - Invalid player state"), ELogVerbosity::Warning);
	}
	OnFailure.Broadcast(ErrorCode);
#endif
}

void UAdvancedSteamUICallbackProxy::OnShowWebUrlClosed(const FString& FinalUrl)
{
	UE_LOG(LogOnline, Log, TEXT("Show Web Url closed with FinalUrl=%s."), *FinalUrl);
	//OnClose.Broadcast(ErrorCode);
}

UAdvancedSteamUICallbackProxy* UAdvancedSteamUICallbackProxy::CreateProxyObjectForShowWebURL(FString InWebURL)
{
	UAdvancedSteamUICallbackProxy* Proxy = NewObject<UAdvancedSteamUICallbackProxy>();
	Proxy->SetFlags(RF_StrongRefOnFrame);
	Proxy->WebURL = InWebURL;
	return Proxy;
}
