// Fill out your copyright notice in the Description page of Project Settings.

#include "Drift.h"
//#include "Magick++.h"

void FDriftProject::StartupModule() {
	/*
	FString DDLPath = FPaths::GamePluginsDir() + FString("/FreeImage/x64/FreeImage.dll");
	DLLHandle = FPlatformProcess::GetDllHandle(*DDLPath);

	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/CORE_RL_bzlib_.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/CORE_RL_cairo_.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/CORE_RL_exr_.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/CORE_RL_flif_.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/CORE_RL_glib_.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/CORE_RL_jp2_.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/CORE_RL_jpeg_.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/CORE_RL_lcms_.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/CORE_RL_libraw_.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/CORE_RL_librsvg_.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/CORE_RL_libxml_.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/CORE_RL_lqr_.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/CORE_RL_Magick++_.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/CORE_RL_MagickCore_.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/CORE_RL_MagickWand_.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/CORE_RL_openjpeg_.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/CORE_RL_pango_.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/CORE_RL_png_.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/CORE_RL_tiff_.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/CORE_RL_ttf_.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/CORE_RL_webp_.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/CORE_RL_zlib_.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/mfc120u.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/msvcp120.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/msvcr120.dll"))));
	DLLHandles.Push(FPlatformProcess::GetDllHandle(*(FPaths::GamePluginsDir() + FString("/ImageMagick/vcomp120.dll"))));

	Magick::InitializeMagick(TCHAR_TO_ANSI(*(FPaths::GamePluginsDir() + FString("/ImageMagick/vcomp120.dll"))));
	*/
}

void FDriftProject::ShutdownModule() {
	/*
	for (int i = 0; i < DLLHandles.Num(); i++) {
		FPlatformProcess::FreeDllHandle(DLLHandles[i]);
	}
	FPlatformProcess::FreeDllHandle(DLLHandle);
	*/
}

IMPLEMENT_PRIMARY_GAME_MODULE(FDriftProject, Drift, "Drift" );
