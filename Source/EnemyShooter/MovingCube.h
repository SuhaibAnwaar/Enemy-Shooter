// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "MovingCube.generated.h"

class UCurveFloat;
class UStaticMeshComponent;

UCLASS()
class ENEMYSHOOTER_API AMovingCube : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovingCube();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	//Static mesh that represents the elevator
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* ElevatorComp;

	//the timeline that will used to move the elevator
	UTimelineComponent* MyTimeline;

	//reference to curve float graph
	UPROPERTY(EditAnywhere, Category = "Timeline")
		UCurveFloat* fCurve;

	//start location of elevator
	UPROPERTY()
		FVector StartLocation;

	//end location of elevatr
	UPROPERTY()
		FVector EndLocation;

	//how much the elevator will move on the z axis
	UPROPERTY(EditAnywhere, Category = "Timeline")
	float ZOffset;

	// Declare our delegate function to be binded with TimelineFloatReturn 
	FOnTimelineFloat InterpFunction{};

	//Delcare our delegat function to be binded with OnTimelineFinished
	FOnTimelineEvent TimelineFinished{};

	//moves mesh on intial run
	UFUNCTION()
		void TimelineFloatReturn(float value);

	//moves mesh back to start and end location on a loop
	UFUNCTION()
		void OnTimelineFinished();

};
