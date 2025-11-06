// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

class UStatiMeshComponent;
class UBoxComponent;
class AKeyComp;
class AEnemyShooterCharacter;

UCLASS()
class ENEMYSHOOTER_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

	//Opening and Closing of door
	void DoorMotion();
	
	//The Key assgined to this door
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Key")
		AKeyComp* Key;
	
	//if this door needs a key
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Key")
		bool NeedsKey;

	//reference to the player
	AEnemyShooterCharacter* PlayerRef;

protected:

	virtual void BeginPlay() override;

	//the frame of the door
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* Frame;

	//the door mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* Door;

	//the collision box that will prevent the player from walking through the door
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UBoxComponent* BoxCollision;

private:
	//Determines if a the door can be opened or closed
	bool isClosed;
};
