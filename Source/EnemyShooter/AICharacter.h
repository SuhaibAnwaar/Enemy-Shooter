// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AICharacter.generated.h" 

class ACheckpoint;
class AEnemyShooterCharacter;
class UWidgetComponent;
class USphereComponent;
class UStaticMeshComponent;
class APlayerCheckpoint;

UCLASS()
class ENEMYSHOOTER_API AAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Initialise componet events
	virtual void PostInitializeComponents() override;

	//Components
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	USphereComponent* SphereCollision;

	//Gun mesh
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* AI_Gun;

	//Location on gun mesh where projectiles should spawn.
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USceneComponent* AI_MuzzleLocation;

	//Location where the gun attaches itself to the AI Mesh
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USceneComponent* Rifle_Hand_Socket;

	//A plane which shows which way the AI is facing on the minimap
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* Arrowicon;

	//Variables
	//Reference to player
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	AEnemyShooterCharacter* PlayerRef;

	//reference to the next next checkpoint the AI needs to walk to
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	ACheckpoint* NextCheckpoint;

	//the projectile class to spawn for when the AI is shooting, used in the controller class
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	TSubclassOf<AActor> ActorToSpawn;

	//has somethin overlapped the AI's sphere collision component
	bool isSphereOverlap;

	//did the AI get hit by the player's projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defaults")
	bool isHit;

	//counts the number of times the AI is hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defaults")
	int Hit;

	//AI's health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defaults")
		float Health;

	//is the AI dead
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defaults")
	bool isDead;

	//is the AI Patrolling
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
		bool isPatrolling;

	//Initial Hearing for enemy
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
		float InitialPawnHearingThreshold;

	//Update hearing for when the player enters the same level as enemy
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
		float UpdatedPawnHearingThreshold;

	//Initial Sight Radius
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
		float InitialSightRadius;

	//how far the enemy can see. This is used used so enemies in different position can detect the enemy more easily and therefore shoot standing further away
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
		float UpdatedSightRadius;

	//holds referecen to player checkpoint class
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	APlayerCheckpoint* PCRef;

	//did the AI get impulse added to it when it died
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
		bool isImpulseApplied;

	//AI fire rate
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
		float ShootingSpeed;

	//Functions 
	// declare Sphere Component overlap begin function
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// declare Sphere Component overlap end function
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//Disable actors collision then after 2 seconds call Die function
	UFUNCTION()
		void Dead();

	//kill AI
	UFUNCTION()
		void Die();
};
