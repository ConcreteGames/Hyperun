// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/**
*	Make TArray<uint8> to upload steam cloud
*/
#include "AdvancedSteamSubsystemUtilsPrivatePCH.h"
#include "AdvancedSteamCloudFile.h"

FAdvancedSteamCloudFile::FAdvancedSteamCloudFile()
{
}


FAdvancedSteamCloudFile::~FAdvancedSteamCloudFile()
{
}

bool FAdvancedSteamCloudFile::VerifyOrCreateDirectory(const FString& BasePath, FString FolderName)
{
	FPaths::NormalizeDirectoryName(FolderName);

	FString Base;
	FString Left;
	FString Remaining;

	//Loop Proteciton
	const int32 MAX_LOOP_ITR = 300; //limit of 3000 directories in the structure

	//Split off the Root
	if (!FolderName.Split(TEXT("/"), &Base, &Remaining))
	{
		return true;
	}

	Base = BasePath + Base + FString("/"); //add root text to Base

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*Base))
	{
		PlatformFile.CreateDirectory(*Base);
	}

	int32 LoopItr = 0;
	while (Remaining != "" && LoopItr < MAX_LOOP_ITR)
	{
		if (!Remaining.Split(TEXT("/"), &Left, &Remaining))
		{
			return true;
		}

		Base += Left + FString("/");

		if (!PlatformFile.DirectoryExists(*Base))
		{
			PlatformFile.CreateDirectory(*Base);
		}

		LoopItr++;
	}

	return true;
}

TSharedPtr<FJsonObject> FAdvancedSteamCloudFile::ParseJSON(const FString& FileContents, const FString& NameForErrors, bool bSilent)
{
	// Load the file up (JSON format)
	if (!FileContents.IsEmpty())
	{
		const TSharedRef< TJsonReader<> >& Reader = TJsonReaderFactory<>::Create(FileContents);

		TSharedPtr<FJsonObject> DescriptorObject;
		if (FJsonSerializer::Deserialize(Reader, /*out*/ DescriptorObject) && DescriptorObject.IsValid())
		{
			// File was loaded and deserialized OK!
			return DescriptorObject;
		}
		else
		{
			if (!bSilent)
			{
				//@TODO: PAPER2D: How to correctly surface import errors to the user?
				UE_LOG(LogNet, Warning, TEXT("Failed to parse tile map JSON file '%s'.  Error: '%s'"), *NameForErrors, *Reader->GetErrorMessage());
			}
			return nullptr;
		}
	}
	else
	{
		if (!bSilent)
		{
			UE_LOG(LogNet, Warning, TEXT("Tile map JSON file '%s' was empty.  This tile map cannot be imported."), *NameForErrors);
		}
		return nullptr;
	}
}

bool FAdvancedSteamCloudFile::SaveReplayFile(const TArray<uint8>& Data)
{
	// Decompress File 
	FArchiveLoadCompressedProxy Decompressor = FArchiveLoadCompressedProxy(Data, ECompressionFlags::COMPRESS_ZLIB);

	// Decompression Error?
	if (Decompressor.GetError())
	{
		UE_LOG(LogOnline, Log, TEXT("FArchiveLoadCompressedProxy>> ERROR : File Was Not Compressed "));
		return false;
	}

	//Decompress
	FBufferArchive DecompressedBinaryArray;
	Decompressor << DecompressedBinaryArray;

	// Read the Data Retrieved by GFileManager
	FMemoryReader FromBinary = FMemoryReader(DecompressedBinaryArray, true); //true, free data after done
	FromBinary.Seek(0);

	int32 ContentsSize;
	TArray<uint8> DemoData;

	TArray<FString> DownloadFileList;

	// get header
	TArray<uint8> Header;
	FromBinary << ContentsSize;
	Header.Empty(ContentsSize);
	Header.AddUninitialized(ContentsSize);
	FromBinary << Header;

	char *Buff = (char *)Header.GetData();
	Buff[ContentsSize] = '\0';

	// get file list
	FString NameForErrors;
	FString HeaderText = FString::Printf(TEXT("%s"), ANSI_TO_TCHAR(Buff));
	TSharedPtr<FJsonObject> JsonObject = ParseJSON(HeaderText, NameForErrors, true);
	if (JsonObject.IsValid())
	{
		FString Value = JsonObject->GetStringField(TEXT("FileCount"));
		int32 FileCount = FCString::Atoi(*Value);
		FString KeyName;
		for (int32 FileIdx = 0; FileIdx < FileCount; ++FileIdx)
		{
			KeyName = FString::Printf(TEXT("FileName_%d"), FileIdx);
			Value = JsonObject->GetStringField(KeyName);
			// Verify CreateDirectory
			if (!VerifyOrCreateDirectory(FPaths::GameSavedDir(), Value))
			{
				UE_LOG(LogOnline, Log, TEXT("SaveReplayFile : Failed to create demo path"));
				return false;
			}

			DownloadFileList.Add(Value);
		}
	}

	// get demo file data 
	for (int32 FileIdx = 0; FileIdx < DownloadFileList.Num(); ++FileIdx)
	{
		FromBinary << ContentsSize;
		DemoData.Empty(ContentsSize);
		DemoData.AddUninitialized(ContentsSize);
		FromBinary << DemoData;

		FString FullFilePath = FPaths::Combine(*(FPaths::GameSavedDir()), *DownloadFileList[FileIdx]);
		if (!FFileHelper::SaveArrayToFile(DemoData, *FullFilePath))
		{
			// Free Binary Array 	
			Decompressor.FlushCache();
			FromBinary.FlushCache();
			return false;
		}
	}

	Decompressor.FlushCache();
	FromBinary.FlushCache();

	// Empty & Close Buffer 
	DecompressedBinaryArray.Empty();
	DecompressedBinaryArray.Close();

	return true;
}

//SaveLoadData
inline void SaveLoadData(FArchive& Ar, int32& SaveDataInt32, TArray<uint8>& SaveData)
{
	Ar << SaveDataInt32;
	Ar << SaveData;
}

void FAdvancedSteamCloudFile::SetHeader(const TArray<FString>& FileList, TArray<uint8>& Header)
{
	// convert json format
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	FString KeyName;
	FString Value = FString::Printf(TEXT("%d"), FileList.Num());
	JsonObject->SetStringField(TEXT("FileCount"), Value);

	for (int32 FileIdx = 0; FileIdx < FileList.Num(); ++FileIdx)
	{
		KeyName = FString::Printf(TEXT("FileName_%d"), FileIdx);
		JsonObject->SetStringField(KeyName, FileList[FileIdx]);
	}

	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	FTCHARToUTF8 PostData(*OutputString);
	int32 Size = PostData.Length();

	Header.Empty(Size);
	Header.AddUninitialized(Size);

	uint8* Data = (uint8*)PostData.Get();
	for (int n = 0; n < Size; ++n)
	{
		Header[n] = Data[n];
	}
}

bool FAdvancedSteamCloudFile::SaveGameDataToFile(const FName& InFileName, const TArray<FString>& FileList)
{
	FBufferArchive ToBinary;
	TArray<uint8> Header;
	SetHeader(FileList, Header);
	int32 ContentsSize = Header.Num();
	SaveLoadData(ToBinary, ContentsSize, Header);

	for (int32 FileIdx = 0; FileIdx < FileList.Num(); ++FileIdx)
	{
		FString FullSavePath = FPaths::Combine(*(FPaths::GameSavedDir()), *FileList[FileIdx]);
		TArray<uint8> FileContents;
		if (FFileHelper::LoadFileToArray(FileContents, *FullSavePath))
		{
			ContentsSize = FileContents.Num();
			SaveLoadData(ToBinary, ContentsSize, FileContents);
		}
		else
		{
			ToBinary.FlushCache();
			ToBinary.Empty();
			return false;
		}
	}

	//No Data
	if (ToBinary.Num() <= 0)
	{
		return false;
	}

	// Compress File 
	//tmp compressed data array
	TArray<uint8> CompressedData;
	FArchiveSaveCompressedProxy Compressor = FArchiveSaveCompressedProxy(CompressedData, ECompressionFlags::COMPRESS_ZLIB);

	//Send entire binary array/archive to compressor
	Compressor << ToBinary;

	//send archive serialized data to binary array
	Compressor.Flush();

	FString FullFilePath = FPaths::Combine(*(FPaths::GameSavedDir()), *FString::Printf(TEXT("%s.zip"), *InFileName.ToString()));
	if (!FFileHelper::SaveArrayToFile(CompressedData, *FullFilePath))
	{
		// Free Binary Array 	
		ToBinary.FlushCache();
		ToBinary.Empty();
		return false;
	}

	// Free Binary Array 	
	ToBinary.FlushCache();
	ToBinary.Empty();

	return true;
}
