// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AICharacter.h"
#include "Checkpoint.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class ENEMYSHOOTER_API ACheckpoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckpoint();

	//component to represent the trigger box
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* BoxComponent;

	//this refers to the next checkpoint, set in unreal editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACheckpoint* NextCheckpoint;

	//Static mesh that is used so this class can be easily moved in the editor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* MeshComp;

	//Initialse componets events
	virtual void PostInitializeComponents() override;

	//when bot enters trigger box, send bot to next checkpoint
	UFUNCTION()
	void OnEnter(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
