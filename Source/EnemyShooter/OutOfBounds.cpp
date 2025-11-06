// Fill out your copyright notice in the Description page of Project Settings.


#include "OutOfBounds.h"
#include "EnemyShooterCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AOutOfBounds::AOutOfBounds()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Trigger box represent a checkpoint
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Respawn Box"));
	BoxCollision->SetupAttachment(GetRootComponent());

	//if player enters trigger box call function
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AOutOfBounds::OnEnter);
}

void AOutOfBounds::OnEnter(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemyShooterCharacter* PlayerRef = Cast<AEnemyShooterCharacter>(OtherActor);

	if (PlayerRef)
	{
		if (PlayerRef->GetCapsuleComponent() == OtherComponent)
		{
			PlayerRef->isDead = true;
		}
	}
}
