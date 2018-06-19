// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"


class FDriftProject : public FDefaultGameModuleImpl
{
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/////Called when GameModule is loaded, load any resources game may need here
	void StartupModule();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/////Called when GameModule is unloaded, before shutdown, unload resources/cleanup here
	void ShutdownModule();
	void* DLLHandle;
	TArray<void*> DLLHandles;
};