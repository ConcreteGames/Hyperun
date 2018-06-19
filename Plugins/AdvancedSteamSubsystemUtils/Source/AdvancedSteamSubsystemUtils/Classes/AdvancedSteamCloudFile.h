// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once
/**
*	Make TArray<uint8> to upload steam cloud
*/

// file has different format in each version
#define ADVANCED_STEAM_CLOUD_VERSION	1

class FAdvancedSteamCloudFile
{
public:
	FAdvancedSteamCloudFile();
	~FAdvancedSteamCloudFile();

	// Did the read succeed
	static bool SaveGameDataToFile(const FName& InFileName, const TArray<FString>& FileList);
	static bool SaveReplayFile(const TArray<uint8>& Data);

private:

	// decompress downloaded file to demo path
	static bool VerifyOrCreateDirectory(const FString& Path, FString FileName);

	static void SetHeader(const TArray<FString>& FileList, TArray<uint8>& Header);
	static TSharedPtr<FJsonObject> ParseJSON(const FString& FileContents, const FString& NameForErrors, bool bSilent);
};