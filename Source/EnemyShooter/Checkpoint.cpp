// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint.h"
#include "Components/BoxComponent.h"

// Sets default values
ACheckpoint::ACheckpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Trigger box represent a checkpoint
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box"));
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->InitBoxExtent(FVector(80.f, 80.f, 90.f));

	//Create Static Mesh
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(BoxComponent);
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
	MeshComp->SetCollisionProfileName(TEXT("OverlapAll"));
	MeshComp->SetHiddenInGame(true);
}

void ACheckpoint::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//if bot enters trigger box call function
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnEnter);
}

void ACheckpoint::OnEnter(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AAICharacter* AICharacter = nullptr;

	if (OtherActor != nullptr)
	{
		AICharacter = Cast<AAICharacter>(OtherActor);
		if (AICharacter != nullptr)
		{
				//set patrol ai to next checkpoint
				AICharacter->NextCheckpoint = NextCheckpoint;
		}
	}
}