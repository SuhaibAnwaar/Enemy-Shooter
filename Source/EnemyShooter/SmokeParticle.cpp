// Fill out your copyright notice in the Description page of Project Settings.


#include "SmokeParticle.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "AICharacter.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ASmokeParticle::ASmokeParticle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create Sphere Collision
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComponent);
	SphereCollision->SetSphereRadius(70.f);
	SphereCollision->SetCollisionProfileName("OverlapAll");

	//Create Particle Component
	Smoke = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Smoke"));
	Smoke->SetupAttachment(SphereCollision);
	
	isOverlap = false;
}

// Called when the game starts or when spawned
void ASmokeParticle::BeginPlay()
{
	Super::BeginPlay();
	
	//die after 10 seconds
	FTimerHandle timer;
	GetWorldTimerManager().SetTimer(timer, this, &ASmokeParticle::Die, 10.0f);

	//Sphere collision overlap events
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ASmokeParticle::OnOverlapBegin);
	
}

// Called every frame
void ASmokeParticle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AIRef)
	{
		//if the AI dies early then destroy Ice as we do not want it to be still freezing in mid air
		if (AIRef->isDead)
		{
			Destroy();
		}
	}
}

void ASmokeParticle::Die()
{
	Destroy();
}

void ASmokeParticle::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
				//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, "Smoke Overlapped");

				//attach itself to the AI so it looks like the AI is freezing
				AttachToActor(AIRef, FAttachmentTransformRules::SnapToTargetIncludingScale);

				isOverlap = true;
			}
		}
	}
}


