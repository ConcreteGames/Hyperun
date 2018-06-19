// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AdvancedSteamSubsystemUtilsPrivatePCH.h"
#include "AdvancedSteamStatsBlueprintLibrary.h"

#include "Private/OnlineSubsystemSteamTypes.h"
#include "Private/OnlineAsyncTaskManagerSteam.h"

#include "OnlineSubsystemSteam.h"

#if !WITH_EDITOR
#include "AdvancedSteam.h"
#endif

UAdvancedSteamStatsBlueprintLibrary::UAdvancedSteamStatsBlueprintLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UAdvancedSteamStatsBlueprintLibrary::SetSteamStat(TArray<FSteamStats> InStatsArray)
{
#if WITH_EDITOR
	return true;
#else
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub && OnlineSub->GetIdentityInterface().IsValid())
	{
		FUniqueNetIdSteam UserId(*OnlineSub->GetIdentityInterface()->GetUniquePlayerId(0).Get());
		if (UserId.IsValid())
		{
			UAdvancedSteam* AdvancedSteam = Cast<UAdvancedSteam>(UAdvancedSteam::StaticClass()->GetDefaultObject());
			if (AdvancedSteam)
			{
				IAdvancedSteamStatsInterfacePtr AdvancedSteamStatsInterface = AdvancedSteam->GetAdvancedSteamStatsInterface();
				if (AdvancedSteamStatsInterface.IsValid())
				{
					TSharedPtr<class FSteamStatsRead, ESPMode::ThreadSafe> ReadObject = MakeShareable(new FSteamStatsRead());
					ReadObject->UserId = FName(*UserId.ToString());
					for (int32 StatIdx = 0; StatIdx < InStatsArray.Num(); StatIdx++)
					{
						new (ReadObject->StatsArray) FSteamStats(InStatsArray[StatIdx]);
					}

					if (AdvancedSteamStatsInterface->SetStats(UserId, ReadObject, false))
					{
						return true;
					}
					else
					{
						FFrame::KismetExecutionMessage(TEXT("SetStat - failed"), ELogVerbosity::Warning);
					}
				}
				else
				{
					FFrame::KismetExecutionMessage(TEXT("SetStat - Invalid or uninitialized AdvancedSteam"), ELogVerbosity::Warning);
				}
			}
			else
			{
				FFrame::KismetExecutionMessage(TEXT("SetStat - Invalid or uninitialized AdvancedSteam"), ELogVerbosity::Warning);
			}
		}
		else
		{
			FFrame::KismetExecutionMessage(TEXT("SetStat - Cannot map local player to unique net ID"), ELogVerbosity::Warning);
		}
	}
	else
	{
		FFrame::KismetExecutionMessage(TEXT("SetStat - Invalid player state"), ELogVerbosity::Warning);
	}
	return false;
#endif
}

bool UAdvancedSteamStatsBlueprintLibrary::IncreaseSteamStat(TArray<FSteamStats> InStatsArray)
{
#if WITH_EDITOR
	return true;
#else
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub && OnlineSub->GetIdentityInterface().IsValid())
	{
		FUniqueNetIdSteam UserId(*OnlineSub->GetIdentityInterface()->GetUniquePlayerId(0).Get());
		if (UserId.IsValid())
		{
			UAdvancedSteam* AdvancedSteam = Cast<UAdvancedSteam>(UAdvancedSteam::StaticClass()->GetDefaultObject());
			if (AdvancedSteam)
			{
				IAdvancedSteamStatsInterfacePtr AdvancedSteamStatsInterface = AdvancedSteam->GetAdvancedSteamStatsInterface();
				if (AdvancedSteamStatsInterface.IsValid())
				{
					TSharedPtr<class FSteamStatsRead, ESPMode::ThreadSafe> ReadObject = MakeShareable(new FSteamStatsRead());
					ReadObject->UserId = FName(*UserId.ToString());
					for (int32 StatIdx = 0; StatIdx < InStatsArray.Num(); StatIdx++)
					{
						new (ReadObject->StatsArray) FSteamStats(InStatsArray[StatIdx]);
					}

					if (AdvancedSteamStatsInterface->SetStats(UserId, ReadObject, true))
					{
						return true;
					}
					else
					{
						FFrame::KismetExecutionMessage(TEXT("IncreamentStat - failed"), ELogVerbosity::Warning);
					}
				}
				else
				{
					FFrame::KismetExecutionMessage(TEXT("IncreamentStat - Invalid or uninitialized AdvancedSteam"), ELogVerbosity::Warning);
				}
			}
		}
		else
		{
			FFrame::KismetExecutionMessage(TEXT("IncreamentStat - Cannot map local player to unique net ID"), ELogVerbosity::Warning);
		}
	}
	else
	{
		FFrame::KismetExecutionMessage(TEXT("IncreamentStat - Invalid player state"), ELogVerbosity::Warning);
	}
	return false;
#endif
}

bool UAdvancedSteamStatsBlueprintLibrary::GetSteamId(FString& SteamId)
{
#if WITH_EDITOR
	return true;
#else
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub && OnlineSub->GetIdentityInterface().IsValid())
	{
		TSharedPtr<const FUniqueNetId> UserId = OnlineSub->GetIdentityInterface()->GetUniquePlayerId(0);
		if (UserId.IsValid())
		{
			SteamId = UserId.Get()->ToString();
			return true;
		}
	}
	return false;
#endif
}

FString UAdvancedSteamStatsBlueprintLibrary::GetCurrentGameLanguage()
{
	ISteamApps* SteamAppsPtr = SteamApps();
	return FString(SteamAppsPtr->GetCurrentGameLanguage());
}
