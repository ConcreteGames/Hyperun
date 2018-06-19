// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AdvancedSteamSubsystemUtilsPrivatePCH.h"
#include "AdvancedSteamLeaderboardBlueprintLibrary.h"


#if !WITH_EDITOR
#include "AdvancedSteam.h"
#endif

//////////////////////////////////////////////////////////////////////////
// UAdvancedSteamLeaderboardBlueprintLibrary
/**
*	Create the proper stat name for a given leaderboard/stat combination
*
* @param LeaderboardName name of leaderboard
* @param StatName name of stat
*/
inline FName GetLeaderboardStatName(const FName& LeaderboardName, const FName& StatName)
{
	return TCHAR_TO_ANSI((*FString::Printf(TEXT("%s_%s"), *LeaderboardName.ToString(), *StatName.ToString())));
}

UAdvancedSteamLeaderboardBlueprintLibrary::UAdvancedSteamLeaderboardBlueprintLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UAdvancedSteamLeaderboardBlueprintLibrary::WriteSteamLeaderboardInteger(
	ELeaderboardUploadMethod LeaderboardUploadMethod,
	FName LeaderboardName, 
	FName StatName, 
	int32 StatValue,
	TArray<int32> InDetails
)
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
				// update steam stats
				IAdvancedSteamStatsInterfacePtr AdvancedSteamStatsInterface = AdvancedSteam->GetAdvancedSteamStatsInterface();
				if (AdvancedSteamStatsInterface.IsValid())
				{
					TSharedPtr<class FSteamStatsRead, ESPMode::ThreadSafe> ReadObject = MakeShareable(new FSteamStatsRead());
					ReadObject->UserId = FName(*UserId.ToString());
					const FString RatedStatName = GetLeaderboardStatName(LeaderboardName, StatName).ToString();
					new (ReadObject->StatsArray) FSteamStats(FName(*RatedStatName), EStatsType::Int32, 0, StatValue);

					// proceed to upload steam leader board in spite of failure of setstats
					if (!AdvancedSteamStatsInterface->SetStats(UserId, ReadObject, false))
					{
						FFrame::KismetExecutionMessage(TEXT("WriteSteamLeaderboardInteger - SetStat - failed"), ELogVerbosity::Warning);
					}
				}
				else
				{
					FFrame::KismetExecutionMessage(TEXT("WriteSteamLeaderboardInteger - Invalid or uninitialized AdvancedSteam"), ELogVerbosity::Warning);
				}

				// upload steam leaderboard
				IAdvancedSteamLeaderboardInterfacePtr AdvancedSteamLeaderboardInterface = AdvancedSteam->GetAdvancedSteamLeaderboardInterface();
				if (AdvancedSteamLeaderboardInterface.IsValid())
				{
					if (AdvancedSteamLeaderboardInterface->UploadLeaderboardScore(LeaderboardUploadMethod, LeaderboardName, StatName, StatValue, InDetails))
					{
						return true;
					}
					else
					{
						FFrame::KismetExecutionMessage(TEXT("WriteSteamLeaderboardInteger - failed"), ELogVerbosity::Warning);
					}
				}
				else
				{
					FFrame::KismetExecutionMessage(TEXT("WriteSteamLeaderboardInteger - Invalid or uninitialized AdvancedSteamLeaderboardInterface"), ELogVerbosity::Warning);
				}
			}
			else
			{
				FFrame::KismetExecutionMessage(TEXT("WriteSteamLeaderboardInteger - Invalid or uninitialized AdvancedSteam"), ELogVerbosity::Warning);
			}
		}
		else
		{
			FFrame::KismetExecutionMessage(TEXT("WriteSteamLeaderboardInteger - Cannot map local player to unique net ID"), ELogVerbosity::Warning);
		}
	}
	else
	{
		FFrame::KismetExecutionMessage(TEXT("WriteSteamLeaderboardInteger - Invalid player state"), ELogVerbosity::Warning);
	}
	return false;
#endif
}

UTexture2D* UAdvancedSteamLeaderboardBlueprintLibrary::GetAvatarIcon(int32 Image)
{
#if WITH_EDITOR
	return nullptr;
#else
	UAdvancedSteam* AdvancedSteam = Cast<UAdvancedSteam>(UAdvancedSteam::StaticClass()->GetDefaultObject());
	if (AdvancedSteam)
	{
		// update steam stats
		IAdvancedSteamLeaderboardInterfacePtr AdvancedSteamLeaderboardInterface = AdvancedSteam->GetAdvancedSteamLeaderboardInterface();
		if (AdvancedSteamLeaderboardInterface.IsValid())
		{
			return AdvancedSteamLeaderboardInterface->GetAvatarIcon(Image);
		}
		else
		{
			FFrame::KismetExecutionMessage(TEXT("GetAvatarIcon - Invalid or uninitialized AdvancedSteam"), ELogVerbosity::Warning);
		}
	}
	else
	{
		FFrame::KismetExecutionMessage(TEXT("GetAvatarIcon - Invalid or uninitialized AdvancedSteam"), ELogVerbosity::Warning);
	}
	return nullptr;
#endif
}
