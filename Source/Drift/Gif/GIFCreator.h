// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
//#include "Magick++.h"

#include <vector.h>



/**
 * 
 */
class UDemoGameInstance;

class DRIFT_API FGIFCreator : public FRunnable
{
public:
	FGIFCreator(UDemoGameInstance* GameInstance, int Width, int Height, int FramePerSec, int TotalFrameCount, int NFramesToRemove, int NColors, int CompressionStrength, bool bByPassCompression);
	virtual~FGIFCreator();

	// Begin FRunnable interface.
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	// End FRunnable interface

	/** Makes sure this thread has stopped properly */
	void EnsureCompletion();

	static bool CreateGifWriter(UDemoGameInstance* GameInstance, int Width, int Height, int FramePerSec, int TotalFrameCount, int NFramesToRemove, int NColors, int CompressionStrength, bool bByPassCompression, FString& GifPath);
	static bool WriteGifFrame(UTextureRenderTarget2D* GifFrame);
	static bool CloseGif();

	/** Shuts down the thread. Static so it can easily be called from outside the thread context */
	static void Shutdown();

	static bool IsThreadFinished();

	bool IsFinished();
	bool ReadPixels(UTextureRenderTarget2D* GifFrame);
	bool CreateLocalFile(FString& OutGifPath);
private:
	/** Thread to run the worker FRunnable on */
	FRunnableThread* Thread;

	/** Singleton instance, can access the thread any time via static accessor, if it is active! */
	static  FGIFCreator* Runnable;

	/** Stop this thread? Uses Thread Safe Counter */
	FThreadSafeCounter StopTaskCounter;

	TArray<TArray<FColor>> RenderTargetArray;
	
	int FrameCurrent;
	int TotalFrameCount;
	int NFramesToRemove;
	static int NColors;
	static int CompressionStrength;
	static bool bByPassCompression;
	int Width;
	int Height;
	int FramePerSec;

	FCriticalSection CritialSection;	
	
	UDemoGameInstance *DemoGameInstance;

	bool IsExceedMaxSize();

	static bool VerifyOrCreateDirectory(const FString& Path, FString FileName);

	// std::vector<Magick::Image> frames;
	static FString GifName;
	//static FString OptimizedGifName;
};
