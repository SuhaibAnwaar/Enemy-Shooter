// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "Components/BoxComponent.h"
#include "EnemyShooterCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Create Door Frame
	Frame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Frame"));
	Frame->SetupAttachment(RootComponent);
	Frame->SetWorldScale3D(FVector(1.0f, 2.0f, 1.0f));

	//Create Door
	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(Frame);
	Door->SetRelativeLocation(FVector(0.0f, 45.f, 0.0f));

	//Create the Door's Collision Box
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(Door);
	BoxCollision->SetWorldScale3D(FVector(0.5f, 1.25f, 2.75f));
	BoxCollision->SetRelativeLocation(FVector(-1.f, -45.f, 100.f));
	BoxCollision->SetCollisionProfileName(TEXT("BlockAll"));

	isClosed = true;
	NeedsKey = false;
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	//get reference
	PlayerRef = Cast<AEnemyShooterCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}
void ADoor::DoorMotion()
{
	//check if this door needs a key to open it 
	if (NeedsKey)
	{
		//Make sure there is a key assgined to this door.
		if (Key)
		{
			//loop through all the keys in the players inventory
			for (int i = 0; i < PlayerRef->KeyCompVariables.Num(); i++)
			{
				//check if player has the same the key assinged to this door
				if (Key == PlayerRef->KeyCompVariables[i])
				{
					//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Key match");

					//if door is closed open and vice versa
					if (isClosed)
					{
						Door->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
						isClosed = false;
					}
					else
					{
						Door->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
						isClosed = true;
					}

				}

			}
		}
	}
	else
	{
		//if door is closed open and vice versa
		if (isClosed)
		{
			Door->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
			isClosed = false;
		}
		else
		{
			Door->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
			isClosed = true;
		}
	}
}