// Fill out your copyright notice in the Description page of Project Settings.

#include "Drift.h"
#include "DemoGameInstance.h"
#include "Engine/DemoNetDriver.h"
#include "Gif/GIFCreator.h"

#define LOCTEXT_NAMESPACE "UDemoGameInstance"

DEFINE_LOG_CATEGORY(LogDemoPlayer);

bool UDemoGameInstance::CreateGif(int Width, int Height, int FramePerSec, int TotalFrameCount, int NFramesToRemove, int NColors, int CompressionStrength, bool bByPassCompression, FString& OutGifPath)
{
	return FGIFCreator::CreateGifWriter(this, Width, Height, FramePerSec, TotalFrameCount, NFramesToRemove, NColors, CompressionStrength, bByPassCompression, OutGifPath);
}

bool UDemoGameInstance::AddGifFrame(UTextureRenderTarget2D* GifFrame)
{
	return FGIFCreator::WriteGifFrame(GifFrame);
}

bool UDemoGameInstance::CloseGif()
{
	return FGIFCreator::CloseGif();
}

bool UDemoGameInstance::IsThreadFinished()
{
	return FGIFCreator::IsThreadFinished();
}

bool UDemoGameInstance::GetGifData(FString GifPath, TArray<uint8>& OutData)
{
	if (FFileHelper::LoadFileToArray(OutData, *GifPath))
	{
		return true;
	}
	return false;
}

FString UDemoGameInstance::GetTwitterConsumerKey()
{
	FString TwitterConsumerKey;
	GConfig->GetString(TEXT("Twitter"), TEXT("ConsumerKey"), TwitterConsumerKey, GGameIni);
	return TwitterConsumerKey;
}

FString UDemoGameInstance::GetTwitterConsumerSecret()
{
	FString TwitterConsumerSecret;
	GConfig->GetString(TEXT("Twitter"), TEXT("ConsumerSecret"), TwitterConsumerSecret, GGameIni);
	return TwitterConsumerSecret;
}
