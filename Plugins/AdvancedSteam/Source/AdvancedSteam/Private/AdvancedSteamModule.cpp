// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AdvancedSteamPrivatePCH.h"
#include "AdvancedSteam.h"
#include "ModuleManager.h"

// FAdvancedSteamModule
#define LOCTEXT_NAMESPACE "AdvancedSteam"

IMPLEMENT_MODULE(FAdvancedSteamModule, AdvancedSteam)

void FAdvancedSteamModule::StartupModule()
{
	UAdvancedSteam::StaticClass();
}

void FAdvancedSteamModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
