// Fill out your copyright notice in the Description page of Project Settings.

#include "Drift.h"
#include "MyClass.h"
#include "Runtime/Engine/Classes/Components/DestructibleComponent.h"
#include "Engine/DestructibleMesh.h"
#include "Components/DestructibleComponent.h"

AMyClass::AMyClass()
{
}

/*void AMyClass::BeginPlay();
{
	Super::BeginPlay();
}

void AMyClass::Tick(float DeltaSeconds);
{
	Super::Tick(DeltaTime);
}*/

void AMyClass::DoSomething(int32 ChunkIndex, bool bInVisible, UDestructibleComponent* TargetMesh)
{
	//Logic
	TargetMesh->SetChunkVisible(ChunkIndex, bInVisible);
}
