// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/PawnSensingComponent.h"
#include "AICharacter.h"
#include "EnemyShooterCharacter.h"
#include "Components/WidgetComponent.h"
#include "Checkpoint.h"
#include "Math/UnrealMathUtility.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "PlayerCheckpoint.h"

AMyAIController::AMyAIController()
{
	//Create Pawn senesing Component
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SetPeripheralVisionAngle(90.f);

	//Initalise variables
	DistanceToPlayer = 0.f;
	WhenShotRotate = false;
	SameGround = true;
	HitTwice = false;
	isNoise = false;
	CountNoise = 0;
}

void AMyAIController::BeginPlay()
{
	Super::BeginPlay();

	//Get Reference to AICharacter
	AICharacter = Cast<AAICharacter>(GetPawn());	

	//set hearing and sight values
	PawnSensing->HearingThreshold = AICharacter->InitialPawnHearingThreshold;
	PawnSensing->LOSHearingThreshold = AICharacter->InitialPawnHearingThreshold;
	PawnSensing->SightRadius = AICharacter->InitialSightRadius;

	//Begin with patrolling
	Patrol();
}

void AMyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//The AI dies when health is 0.1 or below
	if (AICharacter->Health <= 0.1f)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Dead");
		
		//notfiy the AICharacter blueprint so it can hide the health bar
		AICharacter->isDead = true;

		//Call function in AICharacter class so the AI ragdolls and the collision is disabled
		AICharacter->Dead();		
	}

	//check if the player has entered the same level as the enemy by checking if it has overlapped the correct player checkpoint
	if (AICharacter->PCRef)
	{
		//change hearing and sight radius if player is in the same level as the enemy
		if (AICharacter->PCRef->isPlayerOverlap)
		{
			//set the sphere radius to 1.6 times less than the sight radius
			AICharacter->SphereCollision->SetSphereRadius(AICharacter->UpdatedSightRadius / 1.666666666666667f);

			PawnSensing->HearingThreshold = AICharacter->UpdatedPawnHearingThreshold;
			PawnSensing->LOSHearingThreshold = AICharacter->UpdatedPawnHearingThreshold + 100.f;

			PawnSensing->SightRadius = AICharacter->UpdatedSightRadius;
		}
		else//reset hearing and sight radius values
		{
			//set the sphere radius to 1.6 times less than the sight radius
			AICharacter->SphereCollision->SetSphereRadius(AICharacter->InitialSightRadius / 1.666666666666667f);

			PawnSensing->HearingThreshold = AICharacter->InitialPawnHearingThreshold;
			PawnSensing->LOSHearingThreshold = AICharacter->InitialPawnHearingThreshold + 1.f;

			PawnSensing->SightRadius = AICharacter->InitialSightRadius;
		}
	}

	//the enemy is hit while patrolling and the player is outside its Sphere collision,
	//the AI will turn in the direction it was hit from and stand there for 5 seconds
	if (!AICharacter->isSphereOverlap && (AICharacter->isHit || isNoise))
	{	
		FString message = TEXT("Mesh Overlap");
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, message);

		//the AI is hit out of range
		ShotOutOfReach();

		//if AI is hit more than once out of range then it should chase the player 
		HitTwiceOutOfRange();
	}
	else if(AICharacter->isSphereOverlap && (AICharacter->isHit || isNoise))
	{
	
		//how much the AI needs to rotate before facing the player
		FRotator SetLocation_Rot = UKismetMathLibrary::FindLookAtRotation(AICharacter->GetActorLocation(), AICharacter->PlayerRef->GetActorLocation());

		//Only Rotate around Z Axis
		AICharacter->SetActorRotation(FRotator(AICharacter->GetActorRotation().Pitch, SetLocation_Rot.Yaw, AICharacter->GetActorRotation().Roll));
	}

	//Only check distance if the AI is not hit from behind so it can still turn and shoot the player 
	if (!AICharacter->isHit && !isNoise)
	{
		CheckDistance();
	}

	//Check if AI still has a line of sight of player
	LineOfSight();
}

void AMyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AMyAIController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensing->OnSeePawn.AddDynamic(this, &AMyAIController::OnSeePawn);

	PawnSensing->OnHearNoise.AddDynamic(this, &AMyAIController::OnHearNoise);
}

void AMyAIController::ShotOutOfReach()
{
	if (!WhenShotRotate)
	{
		//Disable movement so the AI does not continue moving
		AICharacter->GetCharacterMovement()->DisableMovement();

		//how much the AI needs to rotate before facing the player
		FRotator SetLocation_Rot = UKismetMathLibrary::FindLookAtRotation(AICharacter->GetActorLocation(), AICharacter->PlayerRef->GetActorLocation());

		//Get Random Number between -20 and 20
		float RandomNumber = FMath::RandRange(-20.f, 20.f);

		//Add random number to Yaw Rotation of AI
		SetLocation_Rot.Yaw += RandomNumber;

		//Only Rotate around Z Axis
		AICharacter->SetActorRotation(FRotator(AICharacter->GetActorRotation().Pitch, SetLocation_Rot.Yaw, AICharacter->GetActorRotation().Roll));

		//Set Rotation to true so this part of code only happens once per hit			
		WhenShotRotate = true;

		FTimerHandle Timer;

		//After 5 seconds the AI can contiune patrolling
		GetWorldTimerManager().SetTimer(Timer, this, &AMyAIController::ShotWhilePatrolling, 5.0f, false);

	}
}

void AMyAIController::LineOfSight()
{
	if (AICharacter->isSphereOverlap)
	{
		FVector Loc = AICharacter->GetActorLocation();
		FRotator Rot = GetControlRotation();
		FHitResult Hit;

		FVector Start = Loc;

		//Line Trace will be 1000 units long
		FVector End = Start + (Rot.Vector() * PawnSensing->SightRadius);
		FCollisionQueryParams TraceParams;

		//Draw Line trace
		GetWorld()->LineTraceSingleByChannel(Hit, Start, AICharacter->PlayerRef->GetActorLocation(), ECC_Visibility, TraceParams);

		//Does the line trace hit something
		if (Hit.bBlockingHit)
		{
			//if it hits player then shoot player
			if (Hit.GetActor() == AICharacter->PlayerRef)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Hit Player");
			}
			else // if it does not hit player then move to player
			{
				//Run
				AICharacter->GetCharacterMovement()->MaxWalkSpeed = 500.f;
				//towards player
				MoveToActor(AICharacter->PlayerRef, 1.0f);

				FString message = TEXT("Moving towards player");
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, message);

				AICharacter->isHit = false;
			}
		}
		//DrawDebugLine(GetWorld(), Start, AICharacter->PlayerRef->GetActorLocation(), FColor::Orange, false, 2.0f);
	}
}

void AMyAIController::HitTwiceOutOfRange()
{
	if (AICharacter->Hit >= 2 || CountNoise >= 2)
	{
		//Enable Walking
		AICharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		//Run
		AICharacter->GetCharacterMovement()->MaxWalkSpeed = 500.f;
		//towards player
		MoveToActor(AICharacter->PlayerRef, 1.0f);

		FString m = TEXT("Moving towards player");
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, m);

		HitTwice = true;

		//Reset values
		AICharacter->isHit = false;
		AICharacter->Hit = 0;
		isNoise = false;
		CountNoise = 0;
	}
}

void AMyAIController::ShotWhilePatrolling()
{
	//Reset booleans
	AICharacter->isHit = false;
	WhenShotRotate = false;
	isNoise = false;

	//Enable AI to Walk 
	AICharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);	

	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Blue, "Timer Completed");
}

void AMyAIController::Patrol()
{
	if (AICharacter != nullptr)
	{
		//patrolling speed at 100.f 
		AICharacter->GetCharacterMovement()->MaxWalkSpeed = 100.f;

		//move AI to target point location
		MoveToActor(AICharacter->NextCheckpoint, 5.0f);

		//Try move to location next time 
		//MoveToLocation(AICharacter->NextCheckpoint->GetActorLocation());

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Patrolling");
	}
}

void AMyAIController::CheckDistance()
{
	//Get Distance from AI to player
	DistanceToPlayer = AICharacter->GetDistanceTo(AICharacter->PlayerRef);

	//Get Dot Product between AI and Player
	if (AICharacter->GetDotProductTo(AICharacter->PlayerRef) > 0)
	{
		//Postive, player is infront of AI
		DistanceToPlayer *= 1;
	}
	else
	{
		//Negative, player is behind AI
		DistanceToPlayer *= -1;
	}

	//if player goes out off the bot sight radius, the bot resumes patrolling
	if (DistanceToPlayer > PawnSensing->SightRadius && !HitTwice)
	{
		FString message = TEXT("Player is Too Far Away");
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, message);

		AICharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		AICharacter->isPatrolling = true;
		Patrol();
	}
	else if (DistanceToPlayer < 0)//If Player goes behind the bot then resume patrolling
	{
		FString message = TEXT("Player is behind Bot");
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, message);

		AICharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

		AICharacter->isPatrolling = true;	
		Patrol();
	}
}

void AMyAIController::OnSeePawn(APawn* OtherPawn)
{
	FString message22 = TEXT("Saw Actor ") + OtherPawn->GetName();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, message22);

	//Reset values
	AICharacter->isPatrolling = false;
	HitTwice = false;

	//UE_LOG(LogTemp, Warning, TEXT(" Distance To Player: %.f "), DistanceToPlayer);

	if (AICharacter->PlayerRef != nullptr && AICharacter->Health > 0.0f)
	{
		//check if the player and AI is on the same ground
		if (FMath::RoundToFloat(AICharacter->PlayerRef->GetActorLocation().Z) == FMath::RoundToFloat(AICharacter->GetActorLocation().Z))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Same Level");

			SameGround = true;
		}
		else
		{
			//if the player is not jumping then they must be on a different ground height
			if (AICharacter->PlayerRef->GetCharacterMovement()->IsFalling())
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Jumping");
			}
			else
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Different Level");

				SameGround = false;
			}
		}

		AICharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		FTimerHandle Timer;

		//if sphere collision is true then fire else run towards player until the sphere collision has overlapped
		if (AICharacter->isSphereOverlap == true)
		{
			//if on the same ground as player then shoot otherwise move to the players ground height
			if (SameGround)
			{
				//Stand still and shoot player every second
				GetWorldTimerManager().SetTimer(Timer, AICharacter->ShootingSpeed, false);
				ShootTarget();
				AICharacter->GetCharacterMovement()->MaxWalkSpeed = 0.0f;

				FString message = TEXT("Shooting");
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, message);
			}
			else
			{
				//Run
				AICharacter->GetCharacterMovement()->MaxWalkSpeed = 500.f;
				//towards player
				MoveToActor(AICharacter->PlayerRef, 1.0f);

				FString message = TEXT("Moving towards player");
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, message);

				AICharacter->isHit = false;
				isNoise = false;
			}
		}
		else if (AICharacter->isSphereOverlap == false)
		{
			//Run
			AICharacter->GetCharacterMovement()->MaxWalkSpeed = 500.f;
			//towards player
			MoveToActor(AICharacter->PlayerRef, 1.0f);

			FString message = TEXT("Moving towards player");
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, message);

			AICharacter->isHit = false;
			isNoise = false;
		}
	}

}

void AMyAIController::OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume)
{	
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Hear Noise");
	isNoise = true;
	CountNoise++;
}

void AMyAIController::ShootTarget()
{
	//only be able to shoot if the AI is not dead
	if (!AICharacter->isDead)
	{
		//how much the AI needs to rotate before facing the player
		FRotator SetLocation_Rot = UKismetMathLibrary::FindLookAtRotation(AICharacter->GetActorLocation(), AICharacter->PlayerRef->GetActorLocation());

		//Only Rotate on the Z Axis
		AICharacter->SetActorRotation(FRotator(AICharacter->GetActorRotation().Pitch, SetLocation_Rot.Yaw, AICharacter->GetActorRotation().Roll));

		if (AICharacter->ActorToSpawn)
		{
			//FVector SpawnPos = FVector(AICharacter->GetActorLocation().X, AICharacter->GetActorLocation().Y, AICharacter->GetActorLocation().Z + 50.f);

			//spawn projectile
			AActor* SpawnedProjectile = GetWorld()->SpawnActor<AActor>(AICharacter->ActorToSpawn, AICharacter->AI_MuzzleLocation->GetComponentLocation(), AICharacter->GetActorRotation());

			//check if projectile spawned
			if (SpawnedProjectile)
			{
				FString message = TEXT("isShooting true");
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, message);
			}
			else
			{
				FString message = TEXT("isShooting false");
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, message);
			}
		}
		else
		{
			FString message = TEXT("ActorToSpawn is null ");
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, message);
		}
	}
}