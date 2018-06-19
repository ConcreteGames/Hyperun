// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/Engine/Classes/Components/DestructibleComponent.h"
#include "Components/DestructibleComponent.h"
#include "Engine/DestructibleMesh.h"
#include "GameFramework/Actor.h"
#include "Object.h"
#include "MyClass.generated.h"


/**
 * 
 */

UCLASS(BlueprintType)
class DRIFT_API AMyClass : public AActor
{
	GENERATED_BODY()

public:
	AMyClass();

	//virtual void BeginPlay() override;

	//virtual void Tick( float DeltaSeconds ) override;

	//Expose function to Blueprint
	UFUNCTION(BlueprintCallable, Category = "Coco")
		void DoSomething(int32 ChunkIndex, bool bInVisible, UDestructibleComponent* TargetMesh);
};
