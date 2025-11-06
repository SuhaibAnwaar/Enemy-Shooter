// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerCheckpoint.generated.h"

class UBoxComponent;
class AAICharacter;
class UStaticMeshComponent;

UCLASS()
class ENEMYSHOOTER_API APlayerCheckpoint : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	APlayerCheckpoint();

	//component to represent the trigger box
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UBoxComponent* BoxCollision;

	//Static mesh that is used so this class can be easily moved in the editor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* MeshComp;

	//did the player overlap this checkpoint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
		bool isPlayerOverlap;

	//when the player enters box then set its respawn point to this checkpoint
	UFUNCTION()
		void OnEnter(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
