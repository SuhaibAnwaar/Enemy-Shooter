// Fill out your copyright notice in the Description page of Project Settings.


#include "KeyComp.h"
#include "EnemyShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AKeyComp::AKeyComp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create Component
	KeyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KeyMesh"));
	KeyMesh->SetupAttachment(RootComponent);
	KeyMesh->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
	KeyMesh->SetCollisionProfileName(TEXT("OverlapAll"));

	//Create Key Text
	KeyLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("KeyLabel"));
	KeyLabel->SetupAttachment(KeyMesh);
	KeyLabel->SetText(TEXT("Key"));
	KeyLabel->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));
	KeyLabel->SetRelativeLocation(FVector(0.0f, 0.f, 43.f));
	KeyLabel->HorizontalAlignment = EHorizTextAligment::EHTA_Center;

	isOverlapped = false;
}

// Called when the game starts or when spawned
void AKeyComp::BeginPlay()
{
	Super::BeginPlay();
	
	//Call overlap function when the player overlaps the static mesh
	KeyMesh->OnComponentBeginOverlap.AddDynamic(this, &AKeyComp::OnOverlapBegin);

	//get reference
	PlayerRef = Cast<AEnemyShooterCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

// Called every frame
void AKeyComp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerRef)
	{
		//how much the Text needs to rotate before facing the player
		FRotator SetLocation_Rot = UKismetMathLibrary::FindLookAtRotation(KeyLabel->GetComponentLocation(), PlayerRef->GetActorLocation());

		//Only Rotate on the Z Axis
		KeyLabel->SetWorldRotation(FRotator(KeyLabel->GetRelativeRotation().Pitch, SetLocation_Rot.Yaw, KeyLabel->GetRelativeRotation().Roll));
	}
}

void AKeyComp::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (PlayerRef == OtherActor)
	{
		if (!isOverlapped)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "PlayerRef true");

			//add this key to the player's inventory
			PlayerRef->KeyCompVariables.Add(this);

			KeyMesh->SetHiddenInGame(true);
			KeyLabel->SetHiddenInGame(true);

			//so the player can only have the key added once
			isOverlapped = true;
		}
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "PlayerRef false");
	}
}
