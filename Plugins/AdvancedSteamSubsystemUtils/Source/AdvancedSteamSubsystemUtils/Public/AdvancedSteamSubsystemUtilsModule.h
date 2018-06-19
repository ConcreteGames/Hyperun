// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"
#include "ModuleInterface.h"

class FAdvancedSteamSubsystemUtilsModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static inline FAdvancedSteamSubsystemUtilsModule& Get()
	{
		return FModuleManager::LoadModuleChecked< FAdvancedSteamSubsystemUtilsModule >("AdvancedSteamSubsystemUtils");
	}
};

