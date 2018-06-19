// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SteamStats.h"
#include "AdvancedSteamStatsBlueprintLibrary.generated.h"

UCLASS()
class ADVANCEDSTEAMSUBSYSTEMUTILS_API UAdvancedSteamStatsBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/** Writes the specified Stats */
	UFUNCTION(BlueprintCallable, Category = "Online|SteamStats")
	static bool SetSteamStat(TArray<FSteamStats> StatsArray);
	/** Increases the specified Stats */
	UFUNCTION(BlueprintCallable, Category = "Online|SteamStats")
	static bool IncreaseSteamStat(TArray<FSteamStats> StatsArray);
	/** get steam id */
	UFUNCTION(BlueprintCallable, Category = "Online|SteamStats")
	static bool GetSteamId(FString& SteamId);
	/** Get Current Game Language */
	UFUNCTION(BlueprintCallable, Category = "Online|SteamApps")
	static FString GetCurrentGameLanguage();
};
