// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "LocalizationMaster.generated.h"

/*
class DRIFT_API LocalizationMaster
{
public:

	LocalizationMaster();
	~LocalizationMaster();
};
*/

UCLASS(BlueprintType)
class DRIFT_API ALocalizationMaster : public AActor
{
	GENERATED_BODY()

public:
	//ALocalizationMaster();
	//~ALocalizationMaster();

	UFUNCTION(BlueprintCallable, Category = "Coco")
		//void SetNewCulture(FString Culture2);
		bool SetCurrentCulture(const FString& Culture, const bool SaveToConfig);

	UFUNCTION(BlueprintCallable, Category = "Coco")
		FString GetComputerCulture();

	UFUNCTION(BlueprintCallable, Category = "Coco")
		FString GetCurrentCulture();
};