// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"
#include "ModuleInterface.h"

class IAdvancedSteamInterface;

typedef TSharedPtr<IAdvancedSteamInterface, ESPMode::ThreadSafe> IAdvancedSteamInterfacePtr;

class FAdvancedSteamModule : public IModuleInterface
{
public:	
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static inline FAdvancedSteamModule& Get()
	{
		return FModuleManager::LoadModuleChecked< FAdvancedSteamModule >("AdvancedSteam");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("AdvancedSteam");
	}
};