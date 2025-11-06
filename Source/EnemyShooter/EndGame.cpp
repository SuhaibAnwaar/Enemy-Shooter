// Fill out your copyright notice in the Description page of Project Settings.


#include "EndGame.h"
#include "Components/BoxComponent.h"

// Sets default values
AEndGame::AEndGame()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Trigger box represent a checkpoint
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("EndGame Box"));
	BoxCollision->SetupAttachment(RootComponent);
	BoxCollision->InitBoxExtent(FVector(32.f, 200.f, 100.f));
	BoxCollision->SetCollisionProfileName(TEXT("Custom"));

	//Create Static Mesh
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(BoxCollision);
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
	MeshComp->SetCollisionProfileName(TEXT("OverlapAll"));
	MeshComp->SetHiddenInGame(true);
}

