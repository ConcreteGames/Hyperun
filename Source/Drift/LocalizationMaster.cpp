// Fill out your copyright notice in the Description page of Project Settings.

#include "Drift.h"
#include "LocalizationMaster.h"
#include "Runtime/Core/Public/Internationalization/Internationalization.h"

/*
LocalizationMaster::LocalizationMaster()
{
}

LocalizationMaster::~LocalizationMaster()
{
}
*/

bool ALocalizationMaster::SetCurrentCulture(const FString& Culture, const bool SaveToConfig)
{
	if (FInternationalization::Get().SetCurrentCulture(Culture))
	{
		if (!GIsEditor && SaveToConfig)
		{
			GConfig->SetString(TEXT("Internationalization"), TEXT("Culture"), *Culture, GGameUserSettingsIni);
			GConfig->EmptySection(TEXT("Internationalization.AssetGroupCultures"), GGameUserSettingsIni);
			GConfig->Flush(false, GGameUserSettingsIni);
		}
		return true;
	}

	return false;
}

FString ALocalizationMaster::GetComputerCulture()
{
	return FString(FInternationalization::Get().GetDefaultCulture()->GetName());
}

FString ALocalizationMaster::GetCurrentCulture()
{
	return FString(FInternationalization::Get().GetCurrentCulture()->GetName());
}