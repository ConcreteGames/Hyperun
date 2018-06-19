// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "DemoGameInstance.generated.h"

/**
 * 
 */
DECLARE_LOG_CATEGORY_EXTERN(LogDemoPlayer, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGIFCreatorResult, bool, Success, float, Progress);

UCLASS()
class DRIFT_API UDemoGameInstance : public UGameInstance
{
	GENERATED_BODY()
		
public:
	UFUNCTION(BlueprintCallable, Category = "GifConvert")
		bool CreateGif(int Width, int Height, int FramePerSec, int TotalFrameCount, int NFramesToRemove, int NColors, int CompressionStrength, bool bByPassCompression, FString& OutGifPath);

	UFUNCTION(BlueprintCallable, Category = "GifConvert")
	bool AddGifFrame(UTextureRenderTarget2D* GifFrame);

	UFUNCTION(BlueprintCallable, Category = "GifConvert")
	bool CloseGif();

	UFUNCTION(BlueprintCallable, Category = "GifConvert")
	bool IsThreadFinished();

	UFUNCTION(BlueprintCallable, Category = "GifConvert")
	bool GetGifData(FString GifPath, TArray<uint8>& OutData);

	UPROPERTY(BlueprintAssignable)
	FGIFCreatorResult GIFCreatorResult;

	UFUNCTION(BlueprintCallable, Category = "Tweeter")
	FString GetTwitterConsumerKey();

	UFUNCTION(BlueprintCallable, Category = "Tweeter")
	FString GetTwitterConsumerSecret();
};
