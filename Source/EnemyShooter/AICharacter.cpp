// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnemyShooterCharacter.h"
#include "Checkpoint.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "EnemyShooterProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AAICharacter::AAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	//bot faces direction of acceleration
	GetCharacterMovement()->bOrientRotationToMovement = true;
	//rotation speed
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f);

	//Create Sphere Collision Component to act as the AI's shooting range
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(GetMesh());
	SphereCollision->SetSphereRadius(1000.f);

	//Create Rifle Hand Socket 
	Rifle_Hand_Socket = CreateDefaultSubobject<USceneComponent>(TEXT("Rifle_Hand_Socket"));
	Rifle_Hand_Socket->SetupAttachment(GetMesh());

	//Create Gun Mesh
	AI_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("AI_Gun"));
	AI_Gun->SetupAttachment(Rifle_Hand_Socket);
	AI_Gun->SetRelativeScale3D(FVector(0.9f, 0.9f, 0.9f));

	//Create Muzzle Location
	AI_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("AI_MuzzleLocation"));
	AI_MuzzleLocation->SetupAttachment(Rifle_Hand_Socket);
	AI_MuzzleLocation->SetRelativeLocation(FVector(0.0f, 48.506821f, 9.525437f));

	//Create plane
	Arrowicon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arrowicon"));
	Arrowicon->SetupAttachment(RootComponent);
	Arrowicon->SetRelativeLocation(FVector(0.f, 0.f, 350.f));
	Arrowicon->SetRelativeRotation(FRotator(0.0f, 90.f, 0.f));
	Arrowicon->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));
	Arrowicon->SetCastShadow(false);
	Arrowicon->SetCanEverAffectNavigation(false);	
	Arrowicon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Declare variables
	isSphereOverlap = false;
	isDead = false;
	isHit = false;
	isPatrolling = true;
	Hit = 0;
	Health = 1.0f;
	InitialPawnHearingThreshold = 0.0f;
	UpdatedPawnHearingThreshold = 0.0f;
	InitialSightRadius = 0.0f;
	UpdatedSightRadius = 0.0f;
	ShootingSpeed = 1.0f;
}

// Called when the game starts or when spawned
void AAICharacter::BeginPlay()
{
	Super::BeginPlay();

	//Attach scene component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	Rifle_Hand_Socket->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("index_01_r"));
	
	//Then place the scene component with correct relative rotation and location so that the when the gun is placed in it will 
	//be facing forward
	Rifle_Hand_Socket->SetRelativeRotation(FRotator(0.0f, 110.f, 0.0f));
	Rifle_Hand_Socket->SetRelativeLocation(FVector(5.841635f, 2.126963f, 3.921834f));

	isImpulseApplied = false;
}

// Called every frame
void AAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

// Called to bind functionality to input
void AAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//Sphere collision overlap events
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AAICharacter::OnOverlapBegin);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &AAICharacter::OnOverlapEnd);
}

void AAICharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemyShooterCharacter* Player = Cast<AEnemyShooterCharacter>(OtherActor);
	if (Player)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Overlap Working");
		if (OtherComp == Player->GetCapsuleComponent())
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Overlap Component Working");
			isSphereOverlap = true;
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Overlap Component Not Working");
		}
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Overlap NOT Working");
	}
}

void AAICharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AEnemyShooterCharacter* Player = Cast<AEnemyShooterCharacter>(OtherActor);
	if (Player)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Overlap End Working");
		if (OtherComp == Player->GetCapsuleComponent())
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Overlap End Component Working");
			isSphereOverlap = false;
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Overlap End Component Not Working");
		}
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Overlap End NOT Working");
	}

}

void AAICharacter::Dead()
{
	//Disable actor's collison so the player can walk over its body
	//SetActorEnableCollision(false);
	
	//GetCapsuleComponent()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetSimulatePhysics(true);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (!isImpulseApplied)
	{
		if (PlayerRef)
		{
			GetMesh()->AddImpulse((PlayerRef->GetControlRotation().Vector() * 10000.f), "None", true);
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Called");
			isImpulseApplied = true;
		}
	}

	//Remove AI from world after 2 seconds
	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(Timer, this,  &AAICharacter::Die, 2.0f, false);
	
}
void AAICharacter::Die()
{
	Destroy();
}