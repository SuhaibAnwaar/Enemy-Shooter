// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MyAIController.generated.h"

class AAICharacter;
class UPawnSensingComponent;

UCLASS()
class ENEMYSHOOTER_API AMyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMyAIController();

	virtual void BeginPlay() override;

	//function runs when AI Controller possess bot 
	virtual void OnPossess(APawn* InPawn) override;

	virtual void Tick(float DeltaSeconds) override;

	//Initialse componets events
	virtual void PostInitializeComponents() override;

	//components
	//This gives the AI eyes to detect the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UPawnSensingComponent* PawnSensing;

	//variables
	//Reference to the AI character that is goind to be controlled by this controller class
	AAICharacter* AICharacter;

	//distance form AI to the player
	float DistanceToPlayer;

	//only rotate once when shot
	bool WhenShotRotate;

	//the player and AI is moving on the same ground
	bool SameGround;

	//The AI is shot twice whilst shot out of range
	bool HitTwice;

	//Hears a noise
	bool isNoise;

	//Counts the number of times AI hears a noise
	int CountNoise;

	//functions
	//AI Patrols to and from each checkpoint
	void Patrol();

	//Checks distance between Player and AI to determine if the player is behind or infront of AI and how far away it is
	void CheckDistance();

	//Pawn sensing component detects the Player
	UFUNCTION()
		void OnSeePawn(APawn* OtherPawn);

	//Pawn sensing component hears a sound
	UFUNCTION()
		void OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume);

	//Rotate AI and spawn projectile
	void ShootTarget();

	//rotate roughly towards the direction the AI was shot from and wait 5 seconds
	void ShotWhilePatrolling();

	//the AI is hit but the player is outside the sphere collision
	void ShotOutOfReach();

	//When the AI sees the player a line trace is drawn to make sure it always has a line of sight of the player
	void LineOfSight();

	//If the AI is hit more than once whilst the player is out of range it chases the player
	void HitTwiceOutOfRange();
};
