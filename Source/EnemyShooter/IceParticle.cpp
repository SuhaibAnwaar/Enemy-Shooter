// Fill out your copyright notice in the Description page of Project Settings.


#include "IceParticle.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "AICharacter.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AIceParticle::AIceParticle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create Sphere Collision
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComponent);
	SphereCollision->SetSphereRadius(70.f);
	SphereCollision->SetCollisionProfileName("OverlapAll");

	//Create Particle Component
	Ice = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Ice"));
	Ice->SetupAttachment(SphereCollision);

	isOverlap = false;
}

// Called when the game starts or when spawned
void AIceParticle::BeginPlay()
{
	Super::BeginPlay();
	
	//die after 3 seconds
	FTimerHandle timer;
	GetWorldTimerManager().SetTimer(timer, this, &AIceParticle::Die, 3.0f);

	//Sphere collision overlap events
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AIceParticle::OnOverlapBegin);
}

// Called every frame
void AIceParticle::Tick(float DeltaTime)
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

void AIceParticle::Die()
{
	Destroy();
}

void AIceParticle::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
				//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, "Ice Overlapped");

				//attach itself to the AI so it looks like the AI is freezing
				AttachToActor(AIRef, FAttachmentTransformRules::SnapToTargetIncludingScale);

				isOverlap = true;

				/** Damage is applied to the enemy in the blueprint, IceParticle_BP*/
			}
		}
	}
}

