// Fill out your copyright notice in the Description page of Project Settings.

#include "Drift.h"
#include "GIFCreator.h"

#include "DemoGameInstance.h"
#include "gif.h"
#include "ImageUtils.h"

/*
#include "AllowWindowsPlatformTypes.h"
#include <FreeImage.h>
#include "HideWindowsPlatformTypes.h"

#include "Magick++.h"
*/

#if PLATFORM_WINDOWS
	#include <windows.h>
#endif

//#include "jpge.h"
//#include "jpgd.h"

#define MAX_GIF_SIZE 5131880	// 5MB - 444 X 250 byte

FGIFCreator* FGIFCreator::Runnable = nullptr;
FString FGIFCreator::GifName;

int FGIFCreator::NColors = 128;
int FGIFCreator::CompressionStrength = 80;
bool FGIFCreator::bByPassCompression = false;

//FString FGIFCreator::OptimizedGifName = FString("Optimized.gif");

GifWriter Writer;

FGIFCreator::FGIFCreator(UDemoGameInstance* GameInstance, int InWidth, int InHeight, int InFramePerSec, int InTotalFrameCount, int NFramesToRemove, int NColors, int CompressionStrength, bool bByPassCompression)
	: Width(InWidth),
	Height(InHeight),
	FramePerSec(InFramePerSec),
	FrameCurrent(0),
	TotalFrameCount(InTotalFrameCount),
	NFramesToRemove(NFramesToRemove)
{
	FGIFCreator::NColors = NColors != 0 ? NColors : 64;
	FGIFCreator::CompressionStrength = CompressionStrength != 0 ? CompressionStrength : 150;
	FGIFCreator::bByPassCompression = bByPassCompression;

	DemoGameInstance = GameInstance;
	Writer.f = nullptr;
	Writer.oldImage = nullptr;

	Thread = FRunnableThread::Create(this, TEXT("FGIFCreator"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}

FGIFCreator::~FGIFCreator()
{
	delete Thread;
	Thread = nullptr;
}

//Init
bool FGIFCreator::Init()
{
	return true;
}

//Run
uint32 FGIFCreator::Run()
{
	bool Failed = false;
	FrameCurrent = 0;
	while (StopTaskCounter.GetValue() == 0 && FrameCurrent < TotalFrameCount - NFramesToRemove && 
		// started to write frame and exceed max size
		!(FrameCurrent > 0 && IsExceedMaxSize()))
	{
		TArray<FColor> OutBMP;
		CritialSection.Lock();
		if (RenderTargetArray.Num() > 0)
		{
			OutBMP = RenderTargetArray[0];
			RenderTargetArray.RemoveAt(0);
			++FrameCurrent;
		}
		CritialSection.Unlock();
		if (OutBMP.Num() > 0)
		{
			// FString jpgName = FPaths::GameSavedDir() + FString("/gif-jpegs/") + FString::FromInt(FrameCurrent) + FString(".jpg");
			/*
			
			if (!jpge::compress_image_to_jpeg_file(TCHAR_TO_ANSI(*jpgName), Width, Height, 3, (uint8_t*)OutBMP.GetData()))
			{
				// log_printf("Failed writing to output file!\n");
				FFrame::KismetExecutionMessage(TEXT("Failed writing to output file!"), ELogVerbosity::Warning);
				return EXIT_FAILURE;
			}*/
			/*
			unsigned int bpp = 4*8;
			int pitch = bpp*Width;
			FIBITMAP* src = FreeImage_ConvertFromRawBits((uint8_t*)OutBMP.GetData(), Width, Height, pitch, bpp, 0xFF0000, 0x00FF00, 0x0000FF, false);

			//FIBITMAP *src = FreeImage_ConvertFromRawBitsEx(true, (uint8_t*)OutBMP.GetData(), FIT_BITMAP, Width, Height, pitch, bpp, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);

			FreeImage_Save(FIF_JPEG, src, TCHAR_TO_ANSI(*jpgName), 0);
			FreeImage_Unload(src);
			*/
			/*
			
			Magick::Image image(Width, Height, "RGBA", Magick::CharPixel, (uint8_t*)OutBMP.GetData());
			image.animationDelay(1000 / FramePerSec);
			frames.push_back(image);
			*/
			if (!GifWriteFrame(&Writer, (uint8_t*)OutBMP.GetData(), Width, Height, 100 / FramePerSec, 8, false))
			{
				// notifiy progress
				if (DemoGameInstance)
				{
					DemoGameInstance->GIFCreatorResult.Broadcast(false, TotalFrameCount == 0 ? 0 : (float)FrameCurrent * 100 / (float)TotalFrameCount);
				}
				Failed = true;
				Stop();
				FFrame::KismetExecutionMessage(TEXT("GifWriteFrame Failed"), ELogVerbosity::Warning);
				break;
			}
			OutBMP.Reset();
			
			// notifiy progress
			if (DemoGameInstance)
			{
				DemoGameInstance->GIFCreatorResult.Broadcast(true, TotalFrameCount == 0 ? 0 : (float)FrameCurrent * 100 / (float)TotalFrameCount);
			}
			continue;
		}
		FPlatformProcess::Sleep(0.1);
	}

	// FString gifName = FPaths::GameSavedDir() + FString("art-gif.gif");

	// Magick::writeImages(frames.begin(), frames.end(), TCHAR_TO_ANSI(*gifName), true);

	FrameCurrent = TotalFrameCount;

	return 0;
}

//stop
void FGIFCreator::Stop()
{
	CritialSection.Lock();
	RenderTargetArray.Reset();
	CritialSection.Unlock();

	StopTaskCounter.Increment();
}

void FGIFCreator::EnsureCompletion()
{
	Stop();
	Thread->WaitForCompletion();
}

void FGIFCreator::Shutdown()
{
	if (Runnable)
	{
		Runnable->EnsureCompletion();
		delete Runnable;
		Runnable = nullptr;
	}
}

bool FGIFCreator::IsExceedMaxSize()
{
	/*
	if (nullptr == Writer.f || ftell(Writer.f) >= MAX_GIF_SIZE)
	{
		return true;
	}*/
	return false;
}

bool FGIFCreator::IsFinished()
{
	if ((FrameCurrent >= TotalFrameCount) || IsExceedMaxSize())
	{
		return true;
	}

	return false;
}

bool FGIFCreator::IsThreadFinished()
{
	if (Runnable)
	{
		return Runnable->IsFinished();
	}

	return true;
}

bool FGIFCreator::VerifyOrCreateDirectory(const FString& BasePath, FString FolderName)
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

bool FGIFCreator::CreateGifWriter(UDemoGameInstance* GameInstance, int Width, int Height, int FramePerSec, int TotalFrameCount, int NFramesToRemove, int NColors, int CompressionStrength, bool bByPassCompression, FString& OutGifPath)
{
	//Create new instance of thread if it does not exist
	if (Runnable)
	{
		FGIFCreator::CloseGif();
	}

	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new FGIFCreator(GameInstance, Width, Height, FramePerSec, TotalFrameCount, NFramesToRemove, NColors, CompressionStrength, bByPassCompression);
	}

	if (!VerifyOrCreateDirectory(FPaths::GameSavedDir(), "Demo.gif"))
	{
		return false;
	}
	
	return Runnable->CreateLocalFile(OutGifPath);
}

bool FGIFCreator::CreateLocalFile(FString& OutGifPath)
{
	GifName = FString("Demo.gif");
    FString Separator = UGameplayStatics::GetPlatformName() == FString("Windows") ? "\\" : "/";
	OutGifPath = FPaths::GameSavedDir() + Separator + GifName;
	bool Success = GifBegin(&Writer, TCHAR_TO_ANSI(*OutGifPath), Width, Height, 100 / FramePerSec, 8, false);
	int Retry = 1;
	while (!Success)
	{
		GifName = FString::Printf(TEXT("Demo%d.gif"), Retry++);
		OutGifPath = FPaths::GameSavedDir() + Separator + GifName;
		Success = GifBegin(&Writer, TCHAR_TO_ANSI(*OutGifPath), Width, Height, 100 / FramePerSec, 8, false);
		if (Retry > 10)
			break;
	}
	return Success;
}

bool FGIFCreator::ReadPixels(UTextureRenderTarget2D* GifFrame)
{
	if (Writer.f == nullptr)
	{
		FString GifPath;
		if (!Runnable->CreateLocalFile(GifPath))
		{
			return false;
		}
	}
	TArray<FColor> OutBMP;

	FTextureRenderTargetResource *rtResource = GifFrame->GameThread_GetRenderTargetResource();
	FReadSurfaceDataFlags readPixelFlags(RCM_UNorm);

	OutBMP.AddUninitialized(Width * Height);
	bool Success = rtResource->ReadPixels(OutBMP, readPixelFlags);
	if (Success)
	{
		CritialSection.Lock();
		RenderTargetArray.Add(OutBMP);
		CritialSection.Unlock();
	}

	return Success;
}

bool FGIFCreator::WriteGifFrame(UTextureRenderTarget2D* GifFrame)
{
	if (Runnable)
	{
		Runnable->ReadPixels(GifFrame);
		return true;
	}
	return false;
}

#if PLATFORM_WINDOWS
wchar_t *convertCharArrayToLPCWSTR(const char* charArray)
{
	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}

void windows_system(const FString &programpath, const char *cmdline)
{
	PROCESS_INFORMATION p_info;
	STARTUPINFO s_info;
	//LPCWSTR cmdline, programpath;

	memset(&s_info, 0, sizeof(s_info));
	memset(&p_info, 0, sizeof(p_info));
	s_info.cb = sizeof(s_info);

	//cmdline = _wcsdup(cmd);
	//programpath = _wcsdup(cmd);

	if (CreateProcess(*programpath, convertCharArrayToLPCWSTR(cmdline), NULL, NULL, false, CREATE_NO_WINDOW, NULL, NULL, &s_info, &p_info))
	{
		WaitForSingleObject(p_info.hProcess, INFINITE);
		CloseHandle(p_info.hProcess);
		CloseHandle(p_info.hThread);
	}
}
#endif


bool FGIFCreator::CloseGif()
{
	FGIFCreator::Shutdown();
	if (Writer.f)
	{
		bool write = GifEnd(&Writer);

		if (FGIFCreator::bByPassCompression) {
			return write;
		}
		bool bWindows = UGameplayStatics::GetPlatformName() == FString("Windows");
		FString GifsiclePath = FPaths::ConvertRelativePathToFull(FPaths::Combine(*FPaths::GamePluginsDir(), *FString("giflossy")));
        
        FString GifsicleName = bWindows ? FString("gifsicle") : FString("./gifsicle");
        
		FString GifsicleExe = FPaths::Combine(*GifsiclePath, *GifsicleName);
        
		FString GifPath = FPaths::ConvertRelativePathToFull(FPaths::Combine(*FPaths::GameSavedDir(), *GifName));
		FString Arguments = FString::Printf(TEXT("--batch --colors %d -O3 --lossy=%d "), NColors, CompressionStrength);
        FString ArgumentsWithOutput = Arguments + FString("\"") + GifPath + FString("\"");
        
		//FString OptimizedGifPath = FPaths::ConvertRelativePathToFull(FPaths::Combine(*FPaths::GameSavedDir(), *FString("DemoOptimized.gif")));
		
		//FString Command = FString("\"\"") + GifsiclePath + FString("\" --flip-vertical -O3 --lossy=60 -o \"") + OptimizedGifPath + FString("\" \"") + GifPath + FString("\"\"");
        

		if (bWindows) {
            
#if PLATFORM_WINDOWS
			FString CmdArg = FString("gifsicle.exe ") + ArgumentsWithOutput;
			windows_system(GifsicleExe + FString(".exe"), TCHAR_TO_UTF8(*CmdArg));
#endif
			// WinExec(TCHAR_TO_UTF8(*Command), SW_HIDE);
			//std::system(TCHAR_TO_UTF8(*Command));

			/*
			STARTUPINFOW si;
			PROCESS_INFORMATION pi;
			FString WinCommand = FString("\"\"") + GifsiclePath + FString("\"");
			FString Arguments = FString("--batch --colors 128 --flip-vertical -O3 --lossy=60 \"") + GifPath + FString("\"");
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			ZeroMemory(&pi, sizeof(pi));

			if (CreateProcess(*WinCommand, convertCharArrayToLPCWSTR(TCHAR_TO_UTF8(*Arguments)), NULL, NULL, false, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
			{
				WaitForSingleObject(pi.hProcess, INFINITE);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
			}*/
		} else if (UGameplayStatics::GetPlatformName() == FString("Mac")) {
            
            FString Command = FString("\"") + GifsicleExe + FString("\" ") + ArgumentsWithOutput + FString("");
            FFrame::KismetExecutionMessage(*Command, ELogVerbosity::Warning);
			system(TCHAR_TO_UTF8(*Command));
		}


		return write;
	}

	return true;
}
