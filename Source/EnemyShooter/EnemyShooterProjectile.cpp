// Copyright Epic Games, Inc. All Rights Reserved.

#include "EnemyShooterProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "EnemyShooterCharacter.h"

AEnemyShooterProjectile::AEnemyShooterProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AEnemyShooterProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void AEnemyShooterProjectile::BeginPlay()
{
	Super::BeginPlay();

	//Get player reference
	PlayerRef = Cast<AEnemyShooterCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void AEnemyShooterProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//Get Reference to AI
	AAICharacter* AIRef = Cast<AAICharacter>(OtherActor);
	
	//Check if the projectile hit the AI
	if (AIRef)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "AI Ref true");

		if (PlayerRef)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "Ammo Orginal");

			//Apply damage to the AI
			UGameplayStatics::ApplyDamage(AIRef, 0.1f, nullptr, nullptr, nullptr);
			
			AIRef->isHit = true;

			//increse every time the AI is hit
			AIRef->Hit += 1;

			//UE_LOG(LogTemp, Warning, TEXT(" %i"), AIRef->Hit);
			

			Destroy();
		
		}		
	}	
	else//did not hit the enemy 
	{		
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "Ammo Orginal");	

		Destroy();			
	}

	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}
}


