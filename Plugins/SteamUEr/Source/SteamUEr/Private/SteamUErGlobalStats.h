#pragma once
#pragma push_macro("ARRAY_COUNT")	// duplicate macro definitions in the steam api and the UE api, so we need to do this first
#undef ARRAY_COUNT
#include "ThirdParty/Steamworks/Steamv132/sdk/public/steam/steam_api.h"
#include "ThirdParty/Steamworks/Steamv132/sdk/public/steam/isteamutils.h"
#pragma pop_macro("ARRAY_COUNT")

struct GlobalAchievementData
{

};

class SteamUErGlobalStats
{
protected:
	ISteamUserStats* myStats;

public:
	
	SteamUErGlobalStats()
	{
		myStats = NULL;
	}

	~SteamUErGlobalStats()
	{
		if (myStats)
			delete myStats;
	}

	void RequestGlobalStats(int days);
	void RequestCurrentPlayers();
	void RequestGlobalAchievementPercentages();

	bool globalStatsReady = false;
	bool currentPlayersReady = false;
	bool globalAchievementPercentagesReady = false;

	int32 currentPlayers = 0;
	TArray<GlobalAchievementData> globalAchievementPercentages;


protected:
	
	void OnRequestedGlobalStats(GlobalStatsReceived_t* pCallback, bool bIOFailure);
	CCallResult<SteamUErGlobalStats, GlobalStatsReceived_t> m_callGlobalStatsReceived;

	void OnRequestedCurrentPlayers(NumberOfCurrentPlayers_t* pCallback, bool bIOFailure);
	CCallResult<SteamUErGlobalStats, NumberOfCurrentPlayers_t> m_callCurrentPlayersReceived;

	void OnRequestedGlobalAchievementPercentages(GlobalAchievementPercentagesReady_t* pCallback, bool bIOFailure);
	CCallResult<SteamUErGlobalStats, GlobalAchievementPercentagesReady_t> m_callGlobalAchievementPercentagesReceived;
};