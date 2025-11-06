// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ATurret::ATurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Cube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));
	Cube->SetupAttachment(RootComponent);

	Cone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cone"));
	Cone->SetupAttachment(Cube);

	FiringPoint = CreateDefaultSubobject<UWidgetComponent>(TEXT("FiringPoint"));
	FiringPoint->SetupAttachment(Cone);

	Shooting = true;

}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle Timer;

	GetWorldTimerManager().SetTimer(Timer, this, &ATurret::ShootTarget, 1.0f, true, 1.0f);
	
	
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATurret::ShootTarget()
{
	if (Shooting)
	{
		//how much the turret needs to rotate before facing the player
		FRotator SetLocation_Rot = UKismetMathLibrary::FindLookAtRotation(FiringPoint->GetComponentLocation(), PlayerRef->GetActorLocation());
		SetActorRotation(SetLocation_Rot);

		//Predict where the target is going to be
		//by multiplying the target's velocity with the projectile's velocity
		//then add the target's current location to the distance the target will travel will the projectile is moving
		FVector SpeedPerSecond = PlayerRef->GetActorLocation() + (PlayerRef->GetVelocity() * (GetDistanceTo(PlayerRef) / 3000.f));
		
		//New rotation is the angle of where the target is going to be.
		//FRotator SpawnProjectile_Rot = UKismetMathLibrary::FindLookAtRotation(FiringPoint->GetComponentLocation(), SpeedPerSecond);
		
		FRotator SpawnProjectile_Rot = UKismetMathLibrary::FindLookAtRotation(FiringPoint->GetComponentLocation(), PlayerRef->GetActorLocation());

		float zaza = (GetDistanceTo(PlayerRef) / 3000.f);

		//spawn projectile
		const FVector Location = FiringPoint->GetComponentLocation();
		const FRotator Rotation = SpawnProjectile_Rot;
		GetWorld()->SpawnActor<AActor>(ActorToSpawn, Location, SpawnProjectile_Rot);

		UE_LOG(LogTemp, Warning, TEXT("Location is: %s "), *PlayerRef->GetActorLocation().ToString());
		UE_LOG(LogTemp, Warning, TEXT("Distance To: %s "), *SpeedPerSecond.ToString());
		
	}
}

