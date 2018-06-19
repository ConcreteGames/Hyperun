// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ErrorCodes.h"
#include "SteamStats.generated.h"

UENUM(BlueprintType)
enum class EStatsType : uint8
{
	Int32		UMETA(DisplayName = "Int32"),
	Float		UMETA(DisplayName = "Float"),
};

USTRUCT(BlueprintType)
struct FSteamStats
{
	GENERATED_USTRUCT_BODY()

public:
	FSteamStats() : FloatValue(0) , Int32Value(0)
	{
	}

	FSteamStats(const FName& InName, const EStatsType InStatsType = EStatsType::Int32, const float InFloatValue = 0, const int32 InInt32Value = 0) 
		: KeyName(InName), StatsType(InStatsType), FloatValue(InFloatValue), Int32Value(InInt32Value)
	{
	}

	FSteamStats(const FSteamStats& InStatInfo)
	{
		SetValue(InStatInfo);
	}

	void SetValue(const FSteamStats& InStatInfo)
	{
		KeyName = InStatInfo.KeyName;
		StatsType = InStatInfo.StatsType;
		FloatValue = InStatInfo.FloatValue;
		Int32Value = InStatInfo.Int32Value;
	}

	void AddValue(const FSteamStats& InStatInfo)
	{
		if (StatsType == InStatInfo.StatsType)
		{
			FloatValue += InStatInfo.FloatValue;
			Int32Value += InStatInfo.Int32Value;
		}
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SteamStats)
	FName KeyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SteamStats)
	EStatsType StatsType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SteamStats)
	float FloatValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SteamStats)
	int32 Int32Value;
};

class FSteamStatsRead
{
public:
	// this is a unique key when leaderboard updates
	FName UserId;
	TArray<FSteamStats> StatsArray;

	FSteamStatsRead() :
		UserId(NAME_None)
	{
	}
};

typedef TSharedRef<FSteamStatsRead, ESPMode::ThreadSafe> FSteamStatsReadRef;
typedef TSharedPtr<FSteamStatsRead, ESPMode::ThreadSafe> FSteamStatsReadPtr;