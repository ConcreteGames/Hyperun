// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"
#include "Engine.h"
#include "SteamStats.h"

#include "OnlineSubsystemUtilsClasses.h"
#include "Interfaces/OnlineExternalUIInterface.h"

#include "AdvancedSteamUICallbackProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSteamUIResult, EAdvancedSteamErrorCode, ErrorCode);

UCLASS(MinimalAPI)
class UAdvancedSteamUICallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// Called when there is a successful leaderboard query
	UPROPERTY(BlueprintAssignable)
	FSteamUIResult OnSuccess;

	// Called when there is an unsuccessful leaderboard query
	UPROPERTY(BlueprintAssignable)
	FSteamUIResult OnFailure;

	// Called when WebUI closed - there is no delete delegate in ExternalUIInterface
	//UPROPERTY(BlueprintAssignable)
	//FSteamUIResult OnClose;

	// Queries a leaderboard for an integer value
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", DisplayName="ShowWebURL"), Category="Online|SteamUI")
	static UAdvancedSteamUICallbackProxy* CreateProxyObjectForShowWebURL(FString WebURL);

	virtual void Activate() override;

private:
	void OnShowWebUrlClosed(const FString& FinalUrl);

private:
	FString WebURL;
	EAdvancedSteamErrorCode ErrorCode;
};
