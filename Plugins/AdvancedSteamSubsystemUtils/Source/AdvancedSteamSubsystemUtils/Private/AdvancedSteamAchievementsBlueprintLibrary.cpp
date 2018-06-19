// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AdvancedSteamSubsystemUtilsPrivatePCH.h"
#include "AdvancedSteamAchievementsBlueprintLibrary.h"

#if !WITH_EDITOR
#include "AdvancedSteam.h"
#endif

//////////////////////////////////////////////////////////////////////////
// UAdvancedSteamAchievementsBlueprintLibrary

UAdvancedSteamAchievementsBlueprintLibrary::UAdvancedSteamAchievementsBlueprintLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UAdvancedSteamAchievementsBlueprintLibrary::GetCachedAchievement(FName AchievementId, FSteamAchievements& OutAchievement)
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
				IAdvancedSteamAchievementsInterfacePtr AdvancedSteamAchievementsInterface = AdvancedSteam->GetAdvancedSteamAchievementsInterface();
				if (AdvancedSteamAchievementsInterface.IsValid())
				{
					if (AdvancedSteamAchievementsInterface->GetCachedAchievement(UserId, AchievementId, OutAchievement))
					{
						return true;
					}
					else
					{
						FFrame::KismetExecutionMessage(TEXT("GetCachedAchievement - failed"), ELogVerbosity::Warning);
					}
				}
				else
				{
					FFrame::KismetExecutionMessage(TEXT("GetCachedAchievement - Invalid or uninitialized AdvancedSteam"), ELogVerbosity::Warning);
				}
			}
			else
			{
				FFrame::KismetExecutionMessage(TEXT("GetCachedAchievement - Invalid or uninitialized AdvancedSteam"), ELogVerbosity::Warning);
			}
		}
		else
		{
			FFrame::KismetExecutionMessage(TEXT("GetCachedAchievement - Cannot map local player to unique net ID"), ELogVerbosity::Warning);
		}
	}
	else
	{
		FFrame::KismetExecutionMessage(TEXT("GetCachedAchievement - Invalid player state"), ELogVerbosity::Warning);
	}
	return false;
#endif
}

/** Update stat value if it is progress stat. */
bool UAdvancedSteamAchievementsBlueprintLibrary::SetProgress(FName AchievementId, FSteamStats StatInfo)
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
				IAdvancedSteamAchievementsInterfacePtr AdvancedSteamAchievementsInterface = AdvancedSteam->GetAdvancedSteamAchievementsInterface();
				if (AdvancedSteamAchievementsInterface.IsValid())
				{
					if (AdvancedSteamAchievementsInterface->SetProgress(UserId, AchievementId, StatInfo))
					{
						return true;
					}
					else
					{
						FFrame::KismetExecutionMessage(TEXT("SetProgress - failed"), ELogVerbosity::Warning);
					}
				}
				else
				{
					FFrame::KismetExecutionMessage(TEXT("SetProgress - Invalid or uninitialized AdvancedSteam"), ELogVerbosity::Warning);
				}
			}
			else
			{
				FFrame::KismetExecutionMessage(TEXT("SetProgress - Invalid or uninitialized AdvancedSteam"), ELogVerbosity::Warning);
			}
		}
		else
		{
			FFrame::KismetExecutionMessage(TEXT("SetProgress - Cannot map local player to unique net ID"), ELogVerbosity::Warning);
		}
	}
	else
	{
		FFrame::KismetExecutionMessage(TEXT("SetProgress - Invalid player state"), ELogVerbosity::Warning);
	}
	return false;
#endif
}

UTexture2D* UAdvancedSteamAchievementsBlueprintLibrary::GetAchievementIcon(int32 Image)
{
#if WITH_EDITOR
	return nullptr;
#else
	UAdvancedSteam* AdvancedSteam = Cast<UAdvancedSteam>(UAdvancedSteam::StaticClass()->GetDefaultObject());
	if (AdvancedSteam)
	{
		// update steam stats
		IAdvancedSteamAchievementsInterfacePtr AdvancedSteamAchievementsInterface = AdvancedSteam->GetAdvancedSteamAchievementsInterface();
		if (AdvancedSteamAchievementsInterface.IsValid())
		{
			return AdvancedSteamAchievementsInterface->GetAchievementIcon(Image);
		}
		else
		{
			FFrame::KismetExecutionMessage(TEXT("ClearAchievement - Invalid or uninitialized AdvancedSteam"), ELogVerbosity::Warning);
		}
	}
	else
	{
		FFrame::KismetExecutionMessage(TEXT("ClearAchievement - Invalid or uninitialized AdvancedSteam"), ELogVerbosity::Warning);
	}
	return nullptr;
#endif
}