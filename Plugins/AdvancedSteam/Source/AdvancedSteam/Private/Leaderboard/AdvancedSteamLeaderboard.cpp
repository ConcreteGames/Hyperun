// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AdvancedSteamPrivatePCH.h"
#include "AdvancedSteamLeaderboard.h"

#include "OnlineSubsystemSteam.h"
#include "Private/OnlineAsyncTaskManagerSteam.h"
#include "OnlineAsyncTaskSteamGetLeaderboard.h"
#include "OnlineAsyncTaskSteamGetLeaderboardEntries.h"
#include "OnlineAsyncTaskSteamUploadLeaderboard.h"

FAdvancedSteamLeaderboard::FAdvancedSteamLeaderboard() 
{
}

FAdvancedSteamLeaderboard::~FAdvancedSteamLeaderboard()
{
	PlayerIconImageMap.Reset();
}

bool FAdvancedSteamLeaderboard::DownloadLeaderboardEntries(
	const TArray< TSharedRef<const FUniqueNetId> >& InPlayers, 
	const FSteamLeaderboardReadRef& InReadObject, 
	ELeaderboardRequest LeaderboardDataRequest,
	int32 RangeStart, 
	int32 RangeEnd,
	int32 DetailCount)
{
#if WITH_EDITOR
	return true;
#else
	PlayerIconImageMap.Reset();

	// Find or create handles to all requested leaderboards (async)
	// Will create or retrieve the leaderboards, triggering async calls as appropriate
	CreateLeaderboard(InReadObject->LeaderboardName, ELeaderboardSort::Descending, ELeaderboardFormat::Number);

	FOnlineSubsystemSteam* OnlineSub = (FOnlineSubsystemSteam*)IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
	if (OnlineSub)
	{
		FOnlineAsyncTaskSteamGetLeaderboardEntries* NewStatsTask = new FOnlineAsyncTaskSteamGetLeaderboardEntries(OnlineSub, InPlayers, InReadObject, this, LeaderboardDataRequest, RangeStart, RangeEnd, DetailCount, true);
		OnlineSub->QueueAsyncTask(NewStatsTask);
		return true;
	}
	else
	{
		FFrame::KismetExecutionMessage(TEXT("DownloadLeaderboardEntries - Invalid or uninitialized OnlineSubsystem"), ELogVerbosity::Warning);
	}

	return false;
#endif
}

bool FAdvancedSteamLeaderboard::UploadLeaderboardScore(
	const ELeaderboardUploadMethod LeaderboardUploadMethod, 
	const FName& LeaderboardName,
	const FName& StatName, 
	int32 StatValue, 
	const TArray<int32>& InDetails)
{
#if WITH_EDITOR
	return true;
#else
	// Find or create handles to all requested leaderboards (async)
	// Will create or retrieve the leaderboards, triggering async calls as appropriate
	CreateLeaderboard(LeaderboardName, ELeaderboardSort::Descending, ELeaderboardFormat::Number);

	FOnlineSubsystemSteam* OnlineSub = (FOnlineSubsystemSteam*)IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
	if (OnlineSub)
	{
		// upload score board
		FOnlineAsyncTaskSteamUploadLeaderboard* NewStatsTask = new FOnlineAsyncTaskSteamUploadLeaderboard(OnlineSub, this, LeaderboardUploadMethod, LeaderboardName, StatName, StatValue, InDetails, false);
		OnlineSub->QueueAsyncTask(NewStatsTask);
		return true;
	}
	else
	{
		FFrame::KismetExecutionMessage(TEXT("UploadLeaderboardScore - Invalid or uninitialized OnlineSubsystem"), ELogVerbosity::Warning);
	}

	return false;
#endif
}

UTexture2D* FAdvancedSteamLeaderboard::GetAvatarIcon(const int32 Image)
{
	UTexture2D** TextureImage = PlayerIconImageMap.Find(Image);
	if (nullptr == TextureImage)
	{
		return GetSteamImageAsTexture(Image);
	}

	return *TextureImage;
}

UTexture2D* FAdvancedSteamLeaderboard::GetSteamImageAsTexture(int32 Image)
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
		SteamUtilsPtr->GetImageRGBA(Image, (uint8*)TextureRGBA, 4 * Height * Width);
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

		PlayerIconImageMap.Add(Image, Texture);

		return Texture;
	}

	return nullptr;
}

#if !WITH_EDITOR
FLeaderboardMetadata* FAdvancedSteamLeaderboard::GetLeaderboardMetadata(const FName& LeaderboardName, bool CreateIfNotExist)
{
	FScopeLock ScopeLock(&LeaderboardMetadataLock);
	for (int32 LeaderboardIdx = 0; LeaderboardIdx < Leaderboards.Num(); LeaderboardIdx++)
	{
		const FLeaderboardMetadata& Leaderboard = Leaderboards[LeaderboardIdx];
		if (Leaderboard.LeaderboardName == LeaderboardName)
		{
			return &Leaderboards[LeaderboardIdx];
		}
	}

	if (CreateIfNotExist)
	{
		return CreateLeaderboard(LeaderboardName, ELeaderboardSort::Descending, ELeaderboardFormat::Number);
	}

	return nullptr;
}

FLeaderboardMetadata* FAdvancedSteamLeaderboard::CreateLeaderboard(const FName& LeaderboardName, ELeaderboardSort::Type SortMethod, ELeaderboardFormat::Type DisplayFormat)
{
	FScopeLock ScopeLock(&LeaderboardMetadataLock);
	FLeaderboardMetadata* LeaderboardMetadata = GetLeaderboardMetadata(LeaderboardName);

	// Don't allow multiple attempts to create a leaderboard unless it's actually failed before
	bool bPrevAttemptFailed = (LeaderboardMetadata != nullptr && LeaderboardMetadata->LeaderboardHandle == -1 &&
		(LeaderboardMetadata->AsyncState == EOnlineAsyncTaskState::Done ||
			LeaderboardMetadata->AsyncState == EOnlineAsyncTaskState::Failed));

	if (LeaderboardMetadata == nullptr || bPrevAttemptFailed)
	{
		FOnlineSubsystemSteam* OnlineSub = (FOnlineSubsystemSteam*)IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
		if (OnlineSub)
		{
			FLeaderboardMetadata* NewLeaderboard = new (Leaderboards) FLeaderboardMetadata(LeaderboardName, SortMethod, DisplayFormat);
			NewLeaderboard->AsyncState = EOnlineAsyncTaskState::InProgress;
			OnlineSub->QueueAsyncTask(new FOnlineAsyncTaskSteamGetLeaderboard(OnlineSub, LeaderboardName, SortMethod, DisplayFormat, this));
		}
	}
	// else request already in flight or already found
	return nullptr;
}

void FAdvancedSteamLeaderboard::FindLeaderboard(const FName& LeaderboardName)
{
	FScopeLock ScopeLock(&LeaderboardMetadataLock);
	FLeaderboardMetadata* LeaderboardMetadata = GetLeaderboardMetadata(LeaderboardName);

	// Don't allow multiple attempts to find a leaderboard unless it's actually failed before
	bool bPrevAttemptFailed = (LeaderboardMetadata != nullptr && LeaderboardMetadata->LeaderboardHandle == -1 &&
		(LeaderboardMetadata->AsyncState == EOnlineAsyncTaskState::Done ||
			LeaderboardMetadata->AsyncState == EOnlineAsyncTaskState::Failed));

	if (LeaderboardMetadata == nullptr || bPrevAttemptFailed)
	{
		FOnlineSubsystemSteam* OnlineSub = (FOnlineSubsystemSteam*)IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
		if (OnlineSub)
		{
			// No current find or create in flight
			FLeaderboardMetadata* NewLeaderboard = new (Leaderboards) FLeaderboardMetadata(LeaderboardName);
			NewLeaderboard->AsyncState = EOnlineAsyncTaskState::InProgress;
			OnlineSub->QueueAsyncTask(new FOnlineAsyncTaskSteamGetLeaderboard(OnlineSub, LeaderboardName, this));
		}
	}
	// else request already in flight or already found
}

#endif