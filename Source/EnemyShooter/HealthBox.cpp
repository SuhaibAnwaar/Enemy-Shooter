// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBox.h"
#include "PlayerHealthComponent.h"
#include "EnemyShooterCharacter.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AHealthBox::AHealthBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create Static mesh
	HealthComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HealthComp"));
	HealthComp->SetupAttachment(RootComponent);
	HealthComp->SetSimulatePhysics(false);
	HealthComp->SetCollisionProfileName(TEXT("OverlapAll"));

	//Create Health Label
	HealthLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HealthLabel"));
	HealthLabel->SetupAttachment(HealthComp);
	HealthLabel->SetText(TEXT("Health"));
	HealthLabel->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));
	HealthLabel->SetRelativeLocation(FVector(0.0f, 0.f, 37.877163f));
	HealthLabel->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
}

// Called when the game starts or when spawned
void AHealthBox::BeginPlay()
{
	Super::BeginPlay();
	
	//Call overlap function when the player overlaps the static mesh
	HealthComp->OnComponentBeginOverlap.AddDynamic(this, &AHealthBox::OnOverlapBegin);

	//get reference
	PlayerRef = Cast<AEnemyShooterCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

// Called every frame
void AHealthBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerRef)
	{
		//how much the Text needs to rotate before facing the player
		FRotator SetLocation_Rot = UKismetMathLibrary::FindLookAtRotation(HealthLabel->GetComponentLocation(), PlayerRef->GetActorLocation());

		//Only Rotate on the Z Axis
		HealthLabel->SetWorldRotation(FRotator(HealthLabel->GetRelativeRotation().Pitch, SetLocation_Rot.Yaw, HealthLabel->GetRelativeRotation().Roll));
	}
}

void AHealthBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (PlayerRef == OtherActor)
	{
		FString message = TEXT("Cast success");
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, message);
		
		if (PlayerRef->Health < PlayerRef->DefaultHealth)
		{
			//Update Players Health but make sure it does not exceed the max health limit
			PlayerRef->Health = FMath::Clamp(PlayerRef->Health + 20, 0.0f, PlayerRef->DefaultHealth);

			//UE_LOG(LogTemp, Warning, TEXT("Health: %f"), PlayerRef->Health);
			Destroy();
		}
		else
		{
			FString HealthMessage = TEXT("Health Full");
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, HealthMessage);
		}		
	}
	else
	{
		FString message = TEXT("Cast Failed");
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, message);
	}
}

