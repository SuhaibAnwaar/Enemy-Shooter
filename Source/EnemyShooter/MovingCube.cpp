// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingCube.h"

// Sets default values
AMovingCube::AMovingCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	//Create Static Mesh
	ElevatorComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ElevatorComp"));
	ElevatorComp->SetupAttachment(RootComponent);
	ElevatorComp->SetSimulatePhysics(false);
	ElevatorComp->SetWorldScale3D(FVector(2.0f, 2.0f, 0.25f));

	//create timeline
	MyTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));

	//bind functions
	InterpFunction.BindUFunction(this, FName("TimelineFloatReturn"));
	TimelineFinished.BindUFunction(this, FName("OnTimelineFinished"));

	//set offset
	ZOffset = 1000.0f;
}

// Called when the game starts or when spawned
void AMovingCube::BeginPlay()
{
	Super::BeginPlay();

	//Check if the curve referenece is valid
	if (fCurve)
	{
		//add float curve to timeline and connect to the interp function delegate
		MyTimeline->AddInterpFloat(fCurve, InterpFunction, FName("Alpha"));
		
		//add timeline finished function
		MyTimeline->SetTimelineFinishedFunc(TimelineFinished);

		//set vector location values
		StartLocation = GetActorLocation();
		EndLocation = FVector(StartLocation.X, StartLocation.Y, StartLocation.Z + ZOffset);

		//so the static mesh does not teleport back to starting position after it has moved
		MyTimeline->SetLooping(false);

		//ignore the global time dilation
		MyTimeline->SetIgnoreTimeDilation(true);

		//play timeline
		MyTimeline->Play();
	}
}

void AMovingCube::TimelineFloatReturn(float value)
{
	//move mesh to end location
	SetActorLocation(FMath::Lerp(StartLocation, EndLocation, value));
}

void AMovingCube::OnTimelineFinished()
{
	//if cube is back down to the starting point then move to end location 
	if (MyTimeline->GetPlaybackPosition() == 0.0f)
	{
		//GLog->Log("Play");
		MyTimeline->Play();
	}
	else if(MyTimeline->GetPlaybackPosition() == MyTimeline->GetTimelineLength())//the last key in timeline
	{
		//move mesh back down to starting location
		//GLog->Log("Reverse");
		MyTimeline->Reverse();
	}
	
}

