// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCheckpoint.h"
#include "Components/BoxComponent.h"
#include "EnemyShooterCharacter.h"
#include "AICharacter.h"
#include "Components/CapsuleComponent.h"

// Sets default values
APlayerCheckpoint::APlayerCheckpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Trigger box represent a checkpoint
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Checkpoint Box"));
	BoxCollision->SetupAttachment(RootComponent);
	BoxCollision->InitBoxExtent(FVector(32.f, 200.f, 100.f));
	BoxCollision->SetCollisionProfileName(TEXT("Custom"));

	//if player enters trigger box call function
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &APlayerCheckpoint::OnEnter);

	//Create Static Mesh
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(BoxCollision);
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
	MeshComp->SetCollisionProfileName(TEXT("OverlapAll"));
	MeshComp->SetHiddenInGame(true);

	isPlayerOverlap = false;
}

void APlayerCheckpoint::OnEnter(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemyShooterCharacter* PlayerRef = Cast<AEnemyShooterCharacter>(OtherActor);

	if (PlayerRef)
	{
		//make sure its when the player's capsule componet overlps not the player's box collision
		if (OtherComponent == PlayerRef->GetCapsuleComponent())
		{
			//set player's new checkpoint to this class
			PlayerRef->RespawnCheckpoint = this;

			//set the overlap to initally true
			isPlayerOverlap = true;
			
			//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::White, "Checkpoint");
		}
	}
}
