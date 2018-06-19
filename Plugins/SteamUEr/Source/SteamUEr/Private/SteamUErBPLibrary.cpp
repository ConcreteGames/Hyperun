// 2017 Turfster
// This basically just interfaces with the Steam API, filling in gaps left by the OnlineSubsystemSteam
// (which you need to have installed if you want to make changes, as per https://docs.unrealengine.com/latest/INT/Programming/Online/Steam/)
// PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.

#include "SteamUErPrivatePCH.h"
#pragma push_macro("ARRAY_COUNT")	// duplicate macro definitions in the steam api and the UE api, so we need to do this first
#undef ARRAY_COUNT
#include "ThirdParty/Steamworks/Steamv132/sdk/public/steam/steam_api.h"
#include "ThirdParty/Steamworks/Steamv132/sdk/public/steam/isteamutils.h"
#pragma pop_macro("ARRAY_COUNT")
#include "SteamUErBPLibrary.h"
#include "SteamUErManager.h"

SteamUErGlobalStats* USteamUErBPLibrary::myGlobalStatsObject;
ISteamUserStats* USteamUErBPLibrary::myStats;
ISteamUser* USteamUErBPLibrary::myUser;
bool USteamUErBPLibrary::inShutdownMode;

USteamUErBPLibrary::USteamUErBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	myGlobalStatsObject = new SteamUErGlobalStats();
	inShutdownMode = false;
}

bool USteamUErBPLibrary::SteamUErIsSteamConnected()
{
	if (SteamAPI_Init())
	{
		if (SteamUser())
		{
			return SteamUser()->BLoggedOn();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("SteamUEr: Problem getting Steam user information."));
			return false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SteamUEr: Steam not running?"));
		return false;
	}
}

bool USteamUErBPLibrary::SteamUErInitLeaderboard(FString leaderboardName, int numLeaderboardItems)
{
	return SteamUErManager::Init({ leaderboardName }, numLeaderboardItems);
}

bool USteamUErBPLibrary::SteamUErInitLeaderboards(TArray<FString> leaderboardNames, int numLeaderboardItems)
{
	return SteamUErManager::Init(leaderboardNames, numLeaderboardItems);
}

bool USteamUErBPLibrary::SteamUErInitOrCreateLeaderboards(TArray<FSteamUErLeaderboardCreationData> leaderboardCreationList)
{
	return SteamUErManager::Init(leaderboardCreationList);
}

bool USteamUErBPLibrary::SteamUErCloseLeaderboards()
{
	return SteamUErManager::Close();
}

bool USteamUErBPLibrary::SteamUErPingAsynchronous(bool& scoreSetWaiting, bool& leaderboardGetWaiting, bool& userLeaderboardGetWaiting, bool& globalStatsWaiting, bool& globalPlayerCountWaiting, bool& globalAchievementPercentagesWaiting)
{
	if (!inShutdownMode)
	{
		globalStatsWaiting = SteamUErGlobalStatsReady();
		globalPlayerCountWaiting = SteamUErCurrentPlayersReady();
		//	globalAchievementPercentagesWaiting = SteamUErGlobalAchievementPercentagesReady();
		return SteamUErManager::Update(scoreSetWaiting, leaderboardGetWaiting, userLeaderboardGetWaiting);
	}
	return false;
}

bool USteamUErBPLibrary::SteamUErUploadScoreToLeaderboard(FString leaderboardName, int score)
{
	return SteamUErManager::SetScore(leaderboardName, score);
}

bool USteamUErBPLibrary::SteamUErGetLeaderboard(FString leaderboardName, TArray<FSteamUErLeaderboardData>& leaderboardData)
{
	return SteamUErManager::GetLeaderboardScores(leaderboardName, leaderboardData);
}

bool USteamUErBPLibrary::SteamUErGetLeaderboardInited(FString leaderboardName)
{
	return SteamUErManager::GetLeaderboardInited(leaderboardName);
}

bool USteamUErBPLibrary::SteamUErGetLeaderboardReady(FString leaderboardName)
{
	return SteamUErManager::GetLeaderboardReady(leaderboardName);
}

TArray<UTexture2D*> USteamUErBPLibrary::SteamUErGetLeaderboardAvatars(FString leaderboardName)
{
	return SteamUErManager::GetLeaderboardAvatars(leaderboardName);
}

bool USteamUErBPLibrary::SteamUErRequestLeaderboard(FString leaderboardName, int numScores, ESteamUErScoreTypeEnum leaderboardType)
{
	return SteamUErManager::RequestLeaderboard(leaderboardName, numScores, leaderboardType);
}

bool USteamUErBPLibrary::SteamUErRequestLeaderboardFromLeaderboard(FString leaderboardName, FString userNameSourceLeaderboard)
{
	return SteamUErManager::RequestLeaderboardFromLeaderboard(leaderboardName, userNameSourceLeaderboard);
}

bool USteamUErBPLibrary::SteamUErRequestPlayerFromLeaderboard(FString leaderboardName)
{
	if (SteamAPI_Init())
	{
		if (!myUser) myUser = SteamUser();
		if (myUser)
		{
			CSteamID playerID = myUser->GetSteamID();
			return SteamUErManager::RequestPlayerFromLeaderboard(leaderboardName, playerID);
		}
	}
	return false;
}

bool USteamUErBPLibrary::SteamUErUserLeaderboardDataReady(FString leaderboardName)
{
	return SteamUErManager::GetUserLeaderboardDataReady(leaderboardName);
}

bool USteamUErBPLibrary::SteamUErGetUserLeaderboardData(FString leaderboardName, TArray<FSteamUErLeaderboardData>& leaderboardData)
{
	return SteamUErManager::GetUserLeaderboardData(leaderboardName, leaderboardData);
}


int USteamUErBPLibrary::SteamUErGetPlayMode(UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		if (GEngine != nullptr)
		{
			UWorld* currentWorld = GEngine->GetWorldFromContextObject(WorldContextObject);
			if (currentWorld)
				return (int)currentWorld->WorldType;
		}
	}
	return -1;
}

FString USteamUErBPLibrary::SteamUErConvertDateTimeToString(FDateTime unlockTimestamp) // I could not find a default BP node for this? For some reason?
{
	return unlockTimestamp.ToString();
}


UTexture2D* USteamUErBPLibrary::SteamUErGetPlayerAvatar()
{
	if (SteamAPI_Init())
	{
		if (!myUser) myUser = SteamUser();
		if (myUser)
		{
			CSteamID playerID = myUser->GetSteamID();
			return SteamUErManager::SteamUErGetAvatar(playerID);
		}
	}
	return NULL;
}


UTexture2D* USteamUErBPLibrary::SteamUErGetAchievementIcon(FString achievementName)
{
	UTexture2D* result = NULL;

	if (SteamAPI_Init()) // I don't know if these are strictly *necessary*, but better safe than sorry, right?
	{
		if (!myStats) myStats = SteamUserStats();
		if (myStats)
		{
			int iconHandle = myStats->GetAchievementIcon(TCHAR_TO_UTF8(*achievementName));
			if (iconHandle != 0)
			{
				int bufferSize = 0;
				uint32 width = 0;
				uint32 height = 0;
				if (SteamUtils()->GetImageSize(iconHandle, &width, &height))
				{
					uint8* buffer = new uint8[width*height*4];
					if (SteamUtils()->GetImageRGBA(iconHandle, buffer, width*height * 4))
					{
						result = UTexture2D::CreateTransient(width, height, EPixelFormat::PF_R8G8B8A8); // Create a transient texture
						if (result)
						{
							result->CompressionSettings = TextureCompressionSettings::TC_Default;
							result->SRGB = 0;
							result->UpdateResource();
							FTexture2DMipMap &map = result->PlatformData->Mips[0];
							void* data = map.BulkData.Lock(LOCK_READ_WRITE);
							FMemory::Memcpy(data, buffer, width*height * 4); // write the icon data to the texture
							map.BulkData.Unlock();
							result->UpdateResource();
						}
					}
					delete[] buffer;
				}
			}
		}
	}
	return result;
}

bool USteamUErBPLibrary::SteamUErGetAchievementDisplayAttribute(FString achievementName, FString& localisedName, FString& localisedDescription, bool& isHidden)
{
	if (SteamAPI_Init())
	{
		if (!myStats) myStats = SteamUserStats();
		if (myStats)
		{
			FString achievementKey = "name";
			localisedName = UTF8_TO_TCHAR(myStats->GetAchievementDisplayAttribute(TCHAR_TO_UTF8(*achievementName), TCHAR_TO_UTF8(*achievementKey)));
			achievementKey = "desc";
			localisedDescription = UTF8_TO_TCHAR(myStats->GetAchievementDisplayAttribute(TCHAR_TO_UTF8(*achievementName), TCHAR_TO_UTF8(*achievementKey)));
			achievementKey = "hidden";
			isHidden = (*const_cast<char*>(myStats->GetAchievementDisplayAttribute(TCHAR_TO_UTF8(*achievementName), TCHAR_TO_UTF8(*achievementKey))))=='1';
			return true;
		}
	}
	return false;
}

bool USteamUErBPLibrary::SteamUErGetAchievementUnLockedTime(FString achievementName, bool& unlockStatus, FDateTime& unlockTimestamp)
{
	if (SteamAPI_Init())
	{
		if (!myStats) myStats = SteamUserStats();
		if (myStats)
		{
			uint32 unlockTime = 1;
			myStats->GetAchievementAndUnlockTime(TCHAR_TO_UTF8(*achievementName), &unlockStatus, &unlockTime);
			unlockTimestamp = FDateTime::FromUnixTimestamp(unlockTime);
			return true;
		}
		else
			return false;
	}
	return false;
}

bool USteamUErBPLibrary::SteamUErSetAchievement(FString achievementName)
{
	if (SteamAPI_Init())
	{
		if (!myStats) myStats = SteamUserStats();
		if (myStats != NULL)
		{
			if (myStats->SetAchievement(TCHAR_TO_UTF8(*achievementName)))
			{
				myStats->StoreStats();
				return true;
			}
			else
				UE_LOG(LogTemp, Error, TEXT("Could not set achievement %s"), *achievementName);
		}
		else
			UE_LOG(LogTemp, Error, TEXT("No stats?"));
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Steam not initialized?"));
	return false;
}

bool USteamUErBPLibrary::SteamUErGetAchievement(FString achievementName)
{
	if (SteamAPI_Init())
	{
		if (!myStats) myStats = SteamUserStats();
		if (myStats != NULL)
		{
			bool achieved = false;
			if (myStats->GetAchievement(TCHAR_TO_UTF8(*achievementName),&achieved))
			{
				return achieved;
			}
		}
	}
	return false;
}

bool USteamUErBPLibrary::SteamUErClearAchievement(FString achievementName)
{
	if (SteamAPI_Init())
	{
		if (!myStats) myStats = SteamUserStats();
		if (myStats != NULL)
		{
			if (myStats->ClearAchievement(TCHAR_TO_UTF8(*achievementName)))
			{
				myStats->StoreStats();
				return true;
			}
		}
	}
	return false;
}

int USteamUErBPLibrary::SteamUErNumAchievements()
{
	if (SteamAPI_Init())
	{
		if (!myStats) myStats = SteamUserStats();
		if (myStats != NULL)
		{
			return (myStats->GetNumAchievements());
		}
	}
	return -1;
}

FString USteamUErBPLibrary::SteamUErGetAchievementName(int32 achievementNumber)
{
	if (SteamAPI_Init())
	{
		if (!myStats) myStats = SteamUserStats();
		if (myStats != NULL)
		{
			return UTF8_TO_TCHAR(myStats->GetAchievementName(achievementNumber));
		}
	}
	return "UNKNOWN";
}

bool USteamUErBPLibrary::SteamUErResetAllStats(bool achievementsToo)
{
	if (SteamAPI_Init())
	{
		if (!myStats) myStats = SteamUserStats();
		if (myStats != NULL)
		{
			return (myStats->ResetAllStats(achievementsToo));
		}
	}
	return false;
}

bool USteamUErBPLibrary::SteamUErSetStatInt(FString statName, int32 value)
{
	if (SteamAPI_Init())
	{
		if (!myStats) myStats = SteamUserStats();
		if (myStats!=NULL)
		{
			if (myStats->SetStat(TCHAR_TO_UTF8(*statName), value))
			{
				myStats->StoreStats();
				return true;
			}
			else
				UE_LOG(LogTemp, Error, TEXT("Could not set stat %s"), *statName);
		}
		else
			UE_LOG(LogTemp, Error, TEXT("No stats?"));
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Steam not initialized?"));
	return false;
}

bool USteamUErBPLibrary::SteamUErSetStatFloat(FString statName, float value)
{
	if (SteamAPI_Init())
	{
		if (!myStats) myStats = SteamUserStats();
		if (myStats)
		{
			if (myStats->SetStat(TCHAR_TO_UTF8(*statName), value))
			{
				myStats->StoreStats();
				return true;
			}
			else
				UE_LOG(LogTemp, Log, TEXT("Failed to set stat %s!"), *statName);
		}
	}
	return false;
}

int32 USteamUErBPLibrary::SteamUErGetStatInt(FString statName)
{
	if (SteamAPI_Init())
	{
		if (!myStats) myStats = SteamUserStats();
		if (myStats)
		{
			int32 value = 0;
			if (myStats->GetStat(TCHAR_TO_UTF8(*statName), &value))
			{
				return value;
			}
			else
				UE_LOG(LogTemp, Log, TEXT("Failed to get stat %s!"),*statName);
		}
		else
			return -1;
	}
	return -1;
}

bool USteamUErBPLibrary::SteamUErGetStatIntBool(FString statName, int32& value)
{
	if (SteamAPI_Init())
	{
		if (!myStats) myStats = SteamUserStats();
		if (myStats)
			return myStats->GetStat(TCHAR_TO_UTF8(*statName), &value);
	}
	return false;
}

float USteamUErBPLibrary::SteamUErGetStatFloat(FString statName)
{
	if (SteamAPI_Init())
	{
		if (!myStats) myStats = SteamUserStats();
		if (myStats)
		{
			float value = 0.f;
			myStats->GetStat(TCHAR_TO_UTF8(*statName), &value);
			return value;
		}
		else
			UE_LOG(LogTemp, Log, TEXT("Problem getting stat %s!"), *statName);
	}
	return -1.f;
}

bool USteamUErBPLibrary::SteamUErGetStatFloatBool(FString statName, float& value)
{
	if (SteamAPI_Init())
	{
		if (!myStats) myStats = SteamUserStats();
		if (myStats)
			return myStats->GetStat(TCHAR_TO_UTF8(*statName), &value);
	}
	return false;
}

bool USteamUErBPLibrary::SteamUErRequestGlobalStats(int numDays)
{
	if (myGlobalStatsObject)
	{
		myGlobalStatsObject->RequestGlobalStats(numDays);
		return true;
	}
	UE_LOG(LogTemp, Error, TEXT("SteamUEr: No stats object?!"));
	return false;
}
	

bool USteamUErBPLibrary::SteamUErRequestCurrentPlayers()
{
	if (myGlobalStatsObject)
	{
		myGlobalStatsObject->RequestCurrentPlayers();
		return true;
	}
	UE_LOG(LogTemp, Error, TEXT("SteamUEr: No stats object?!"));
	return false;
}
/*
bool USteamUErBPLibrary::SteamUErRequestAchievementPercentages()
{
	if (!myGlobalStatsObject)
		myGlobalStatsObject = new SteamUErGlobalStats();

	myGlobalStatsObject->RequestGlobalAchievementPercentages();
	return true;
}*/

bool USteamUErBPLibrary::SteamUErCurrentPlayersReady()
{
	if (myGlobalStatsObject)
	{
		return myGlobalStatsObject->currentPlayersReady;
	}
	return false;
}

bool USteamUErBPLibrary::SteamUErGlobalStatsReady()
{
	if (!myGlobalStatsObject)
	{
		UE_LOG(LogTemp, Error, TEXT("SteamUEr: Global stats object not initialized before global stats ready called!"));
		myGlobalStatsObject = new SteamUErGlobalStats();
		// obviously not
		return false;
	}

	return myGlobalStatsObject->globalStatsReady;
}

/*
bool USteamUErBPLibrary::SteamUErGlobalAchievementPercentagesReady()
{
	if (myGlobalStatsObject)
	{
		return myGlobalStatsObject->globalAchievementPercentagesReady;
	}
	return false;
}*/

int USteamUErBPLibrary::SteamUErCurrentPlayers()
{
	if (myGlobalStatsObject)
	{
		if (myGlobalStatsObject->currentPlayersReady)
			return myGlobalStatsObject->currentPlayers;
	}
	return -1;
}

void USteamUErBPLibrary::SteamUErInvalidateGlobalStats()
{
	if (myGlobalStatsObject)
	{
		myGlobalStatsObject->globalStatsReady = false;
	}
}

bool USteamUErBPLibrary::SteamUErGetGlobalStatIntBool(FString statName, FString& value)
{
	if (SteamAPI_Init())
	{
		if (!myStats) myStats = SteamUserStats();
		if (myStats)
		{
			int64 integerValue = 0;
			bool success = myStats->GetGlobalStat(TCHAR_TO_UTF8(*statName), &integerValue);
			char buffer[20];
			sprintf(buffer, "%lli", integerValue);
			value = FString(UTF8_TO_TCHAR(buffer));
			return success;
		}
	}
	return false;

}

bool USteamUErBPLibrary::SteamUErGetGlobalStatFloatBool(FString statName, float& value)
{
	if (SteamAPI_Init())
	{
		if (!myStats) myStats = SteamUserStats();
		if (myStats)
			return myStats->GetStat(TCHAR_TO_UTF8(*statName), &value);
	}
	return false;
}