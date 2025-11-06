// Fill out your copyright notice in the Description page of Project Settings.


#include "FireParticle.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "AICharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AFireParticle::AFireParticle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create Sphere Collision
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComponent);
	SphereCollision->SetSphereRadius(70.f);
	SphereCollision->SetCollisionProfileName("OverlapAll");

	//Create Particle Component
	Fire = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Fire"));
	Fire->SetupAttachment(SphereCollision);

	//initalise variable
	isOverlap = false;
}

// Called when the game starts or when spawned
void AFireParticle::BeginPlay()
{
	Super::BeginPlay();
	
	//die after 3 seconds
	FTimerHandle timer;
	GetWorldTimerManager().SetTimer(timer, this, &AFireParticle::Die, 3.0f);

	//Sphere collision overlap events
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AFireParticle::OnOverlapBegin);

}

// Called every frame
void AFireParticle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AIRef)
	{
		//if the AI dies early then destroy fire as we do not want it to be still burning in mid air
		if (AIRef->isDead)
		{
			Destroy();
		}
	}
}

void AFireParticle::Die()
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, "Fire Dead");
	
	Destroy();
}

void AFireParticle::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AIRef = Cast<AAICharacter>(OtherActor);
	if (AIRef)
	{
		//make sure the overlap is with the enemy's capusle not its sphere collision
		if (AIRef->GetCapsuleComponent() == OtherComp)
		{
			//make sure it tries to attach itself once not several times as this would cause an infinite loop
			if (!isOverlap)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, "Fire Overlapped");

				//attach itself to the AI so it looks like the AI is burning
				AttachToActor(AIRef, FAttachmentTransformRules::SnapToTargetIncludingScale);

				isOverlap = true;
				
				/** Damage is applied to the enemy in the blueprint, FireParticle_BP*/
			}
		}
	}
}

