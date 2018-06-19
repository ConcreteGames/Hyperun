// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AdvancedSteamPrivatePCH.h"
#include "AdvancedSteamAchievements.h"

#include "AdvancedSteam.h"
#include "Interfaces/AdvancedSteamStatsInterface.h"
#include "OnlineAsyncTaskSteamRetrieveAchievements.h"
#include "OnlineAsyncTaskSteamUpdateAchievements.h"
#include "OnlineSubsystemSteam.h"
#include "Private/OnlineSubsystemSteamTypes.h"
#include "Private/OnlineAsyncTaskManagerSteam.h"


FAdvancedSteamAchievements::FAdvancedSteamAchievements()
{
	ReadAchievements(Achievements);
}

FAdvancedSteamAchievements::~FAdvancedSteamAchievements()
{
	IconImageMap.Reset();
	PlayerAchievements.Reset();
}

/** Returns empty string if couldn't read */
FString FAdvancedSteamAchievements::GetKey(const FString& KeyName)
{
	FString Result;
	if (!GConfig->GetString(TEXT("OnlineSubsystemSteam"), *KeyName, Result, GEngineIni))
	{
		return TEXT("");	// could just return Result, but being explicit is better
	}
	return Result;
}

bool FAdvancedSteamAchievements::ReadAchievements(TArray<FSteamAchievementsPtr> &OutArray)
{
	OutArray.Empty();
	int NumAchievements = 0;

	for (;; ++NumAchievements)
	{
		FString Id = GetKey(FString::Printf(TEXT("Achievement_%d_Id"), NumAchievements));
		if (Id.IsEmpty())
		{
			break;
		}
		FSteamAchievementsPtr NewAch = MakeShareable(new FSteamAchievements());
		NewAch->AchievementId = FName(*Id);
		NewAch->IsProgressAchievement = GetKey(FString::Printf(TEXT("Achievement_%d_IsProgress"), NumAchievements)).ToLower() == "true";
		// set progress stats information
		if (NewAch->IsProgressAchievement)
		{
			NewAch->StatInfo.KeyName = FName(*GetKey(FString::Printf(TEXT("Achievement_%d_Progress_StatName"), NumAchievements)));
			if (GetKey(FString::Printf(TEXT("Achievement_%d_Progress_Type"), NumAchievements)) == "Float")
			{
				NewAch->StatInfo.StatsType = EStatsType::Float;
			}
			else
			{
				NewAch->StatInfo.StatsType = EStatsType::Int32;
			}
		}

		OutArray.Add(NewAch);
	}

	return NumAchievements > 0;
}

bool FAdvancedSteamAchievements::QueryAchievements(const FUniqueNetIdSteam& InUserId)
{
	// precache achievements
	FOnlineSubsystemSteam* OnlineSub = (FOnlineSubsystemSteam*)IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
	if (OnlineSub)
	{
		// upload score board
		FOnlineAsyncTaskSteamRetrieveAchievements* NewStatsTask = new FOnlineAsyncTaskSteamRetrieveAchievements(OnlineSub, InUserId, true, this);
		OnlineSub->QueueAsyncTask(NewStatsTask);
		return true;
		}
	else
	{
		FFrame::KismetExecutionMessage(TEXT("QueryAchievements - Invalid or uninitialized OnlineSubsystem"), ELogVerbosity::Warning);
	}
	
	return false;
}

bool FAdvancedSteamAchievements::GetAchievements(const FUniqueNetIdSteam& InUserId, TArray<FSteamAchievements>& OutAchievementsArray)
{
	const TArray<FSteamAchievementsPtr> * PlayerAch = PlayerAchievements.Find(InUserId);
	if (NULL == PlayerAch)
	{
		// achievements haven't been read for a player
		UE_LOG(LogOnline, Warning, TEXT("Steam achievements have not been read for player %s"), *InUserId.ToString());
		return false;
	}
	for (int32 AchIdx = 0; AchIdx < PlayerAch->Num(); ++AchIdx)
	{
		FSteamAchievementsPtr Achievement = (*PlayerAch)[AchIdx];
		OutAchievementsArray.Add(*Achievement.Get());
	}

	return true;
}

bool FAdvancedSteamAchievements::GetCachedAchievement(const FUniqueNetIdSteam& InUserId, const FName& AchievementId, FSteamAchievements& OutAchievement)
{
	const TArray<FSteamAchievementsPtr> * PlayerAch = PlayerAchievements.Find(InUserId);
	if (NULL == PlayerAch)
	{
		// achievements haven't been read for a player
		UE_LOG(LogOnline, Warning, TEXT("Steam achievements have not been read for player %s"), *InUserId.ToString());
		return EOnlineCachedResult::NotFound;
	}

	const int32 AchNum = PlayerAch->Num();
	for (int32 AchIdx = 0; AchIdx < AchNum; ++AchIdx)
	{
		if ((*PlayerAch)[AchIdx]->AchievementId == AchievementId)
		{
			OutAchievement = *(*PlayerAch)[AchIdx];
			return true;
		}
	}

	// no such achievement
	UE_LOG(LogOnline, Warning, TEXT("Could not find Steam achievement '%s' for player %s"), *AchievementId.ToString(), *InUserId.ToString());

	return false;
}


UTexture2D* FAdvancedSteamAchievements::GetAchievementIcon(const int32 Image)
{
	UTexture2D** TextureImage = IconImageMap.Find(Image);
	if (nullptr == TextureImage)
	{
		return GetSteamImageAsTexture(Image);
	}
	return *TextureImage;
}

UTexture2D* FAdvancedSteamAchievements::GetSteamImageAsTexture(int32 Image)
{
	if (0 == Image)
	{
		return nullptr;
	}

	ISteamUtils* SteamUtilsPtr = SteamUtils();
	check(SteamUtilsPtr);

	uint32 Width;
	uint32 Height;

	SteamUtilsPtr->GetImageSize(Image, &Width, &Height);


	if (Width > 0 && Height > 0)
	{
		// Creating the buffer "TextureRGBA" and then filling it with the RGBA Stream from the Steam Texture
		uint8 *TextureRGBA = new uint8[Width * Height * 4];

		// Filling the buffer with the RGBA Stream from the Steam Texture and creating a UTextur2D to parse the RGBA Steam in
		SteamUtilsPtr->GetImageRGBA(Image, (uint8*)TextureRGBA, 4 * Height * Width * sizeof(char));
		UTexture2D* Texture = UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);

		uint8* MipData = (uint8*)Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(MipData, (void*)TextureRGBA, Height * Width * 4);
		Texture->PlatformData->Mips[0].BulkData.Unlock();
		delete[] TextureRGBA;

		// Setting some Parameters for the Texture and finally returning it
		Texture->PlatformData->NumSlices = 1;
		Texture->NeverStream = true;
		Texture->CompressionSettings = TC_EditorIcon;

		Texture->UpdateResource();
		IconImageMap.Add(Image, Texture);

		return Texture;
	}

	return nullptr;
}

bool FAdvancedSteamAchievements::UpdateAchievementsForUser(const FUniqueNetIdSteam& PlayerId, EAdvancedSteamErrorCode &ErrorCode)
{
	IconImageMap.Reset();

	// shouldn't get this far if no achievements are configured
	ISteamUserStats* SteamUserStatsPtr = SteamUserStats();
	check(SteamUserStatsPtr);
	CSteamID SteamUserId = PlayerId;

	// Get achievement name iAchievement in [0,GetNumAchievements)
	// new array
	TArray<FSteamAchievementsPtr> AchievementsForPlayer;
	const int32 AchNum = Achievements.Num();

	for (int32 AchIdx = 0; AchIdx < AchNum; ++AchIdx)
	{
		// get the info
		FSteamAchievementsPtr NewAch = Achievements[AchIdx];

		uint32 UnlockUnixTime;
		if (!SteamUserStatsPtr->GetAchievementAndUnlockTime(TCHAR_TO_UTF8(*NewAch->AchievementId.ToString()), &NewAch->UnLocked, &UnlockUnixTime))
		{
			UE_LOG(LogOnline, Warning, TEXT("GetAchievementAndUnlockTime() failed for achievement '%s'"), *NewAch->AchievementId.ToString());
			// skip this achievement
			continue;
		}
		SteamUserStatsPtr->GetAchievement(TCHAR_TO_UTF8(*NewAch->AchievementId.ToString()), &NewAch->IsCleared);

		NewAch->UnlockTime = FDateTime::FromUnixTimestamp(UnlockUnixTime);

		NewAch->Title = FText::FromString(UTF8_TO_TCHAR(SteamUserStatsPtr->GetAchievementDisplayAttribute(TCHAR_TO_UTF8(*NewAch->AchievementId.ToString()), "name")));
		NewAch->Desc = FText::FromString(UTF8_TO_TCHAR(SteamUserStatsPtr->GetAchievementDisplayAttribute(TCHAR_TO_UTF8(*NewAch->AchievementId.ToString()), "desc")));

		NewAch->IsHidden = FCString::Atoi(UTF8_TO_TCHAR(SteamUserStatsPtr->GetAchievementDisplayAttribute(TCHAR_TO_UTF8(*NewAch->AchievementId.ToString()), "hidden"))) != 0;

		// set icon
		NewAch->IconImage = SteamUserStatsPtr->GetAchievementIcon(TCHAR_TO_UTF8(*NewAch->AchievementId.ToString()));
		UTexture2D* TextureImage = GetAchievementIcon(NewAch->IconImage);
		if (!TextureImage)
		{
			NewAch->IconImage = 0;
		}

		// set progress stats information
		if (NewAch->IsProgressAchievement)
		{
			bool bSuccess = true;
			switch (NewAch->StatInfo.StatsType)
			{
			case EStatsType::Int32:
			{
				if (!SteamUserStatsPtr->GetUserStat(SteamUserId, TCHAR_TO_UTF8(*NewAch->StatInfo.KeyName.ToString()), &NewAch->StatInfo.Int32Value))
				{
					bSuccess = false;
					ErrorCode = EAdvancedSteamErrorCode::STEAM_STATS_INVALID;
				}
				break;
			}
			case EStatsType::Float:
			{
				if (!SteamUserStatsPtr->GetUserStat(SteamUserId, TCHAR_TO_UTF8(*NewAch->StatInfo.KeyName.ToString()), &NewAch->StatInfo.FloatValue))
				{
					bSuccess = false;
					ErrorCode = EAdvancedSteamErrorCode::STEAM_STATS_INVALID;
				}
				break;
			}
			default:
				UE_LOG_ONLINE(Warning, TEXT("Skipping unsuppported key value pair to Steam %s"), *NewAch->StatInfo.KeyName.ToString());
				bSuccess = false;
				ErrorCode = EAdvancedSteamErrorCode::UNSUPPORTED_TYPE;
				break;
			}

			if (!bSuccess)
			{
				UE_LOG_ONLINE(Warning, TEXT("Failure to get key value pair when getting to Steam %s"), *NewAch->StatInfo.KeyName.ToString());
				return false;
			}
		}

		AchievementsForPlayer.Add(NewAch);
	}

	// should replace any already existing values
	PlayerAchievements.Add(PlayerId, AchievementsForPlayer);

	return true;
}

bool FAdvancedSteamAchievements::SetProgress(const FUniqueNetIdSteam& InUserId, const FName& AchievementId, const FSteamStats& StatInfo)
{
	UAdvancedSteam* AdvancedSteam = Cast<UAdvancedSteam>(UAdvancedSteam::StaticClass()->GetDefaultObject());
	if (AdvancedSteam)
	{
		IAdvancedSteamStatsInterfacePtr AdvancedSteamStatsInterface = AdvancedSteam->GetAdvancedSteamStatsInterface();
		if (AdvancedSteamStatsInterface.IsValid())
		{
			// update progress stat
			TSharedPtr<class FSteamStatsRead, ESPMode::ThreadSafe> ReadObject = MakeShareable(new FSteamStatsRead());
			ReadObject->UserId = FName(*InUserId.ToString());
			new (ReadObject->StatsArray) FSteamStats(StatInfo);

			if (AdvancedSteamStatsInterface->SetStats(InUserId, ReadObject, false))
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
	return true;
}

bool FAdvancedSteamAchievements::SetAchievement(const FUniqueNetIdSteam& InUserId, const FName& AchievementId)
{
	// precache achievements
	FOnlineSubsystemSteam* OnlineSub = (FOnlineSubsystemSteam*)IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
	if (OnlineSub)
	{
		// upload score board
		FOnlineAsyncTaskSteamUpdateAchievements* NewStatsTask = new FOnlineAsyncTaskSteamUpdateAchievements(OnlineSub, InUserId, AchievementId, false, true, this);
		OnlineSub->QueueAsyncTask(NewStatsTask);
		return true;
	}
	else
	{
		FFrame::KismetExecutionMessage(TEXT("SetAchievement - Invalid or uninitialized OnlineSubsystem"), ELogVerbosity::Warning);
	}
	return false;
}

bool FAdvancedSteamAchievements::ClearAchievement(const FUniqueNetIdSteam& InUserId, const FName& AchievementId)
{
	// precache achievements
	FOnlineSubsystemSteam* OnlineSub = (FOnlineSubsystemSteam*)IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
	if (OnlineSub)
	{
		// upload score board
		FOnlineAsyncTaskSteamUpdateAchievements* NewStatsTask = new FOnlineAsyncTaskSteamUpdateAchievements(OnlineSub, InUserId, AchievementId, true, true, this);
		OnlineSub->QueueAsyncTask(NewStatsTask);
		return true;
	}
	else
	{
		FFrame::KismetExecutionMessage(TEXT("ClearAchievement - Invalid or uninitialized OnlineSubsystem"), ELogVerbosity::Warning);
	}
	return false;
}