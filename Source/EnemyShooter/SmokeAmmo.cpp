// Fill out your copyright notice in the Description page of Project Settings.


#include "SmokeAmmo.h"
#include "EnemyShooterCharacter.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASmokeAmmo::ASmokeAmmo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create Static Mesh
	SmokeComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SmokeComp"));
	SmokeComp->SetupAttachment(RootComponent);
	SmokeComp->SetSimulatePhysics(false);
	SmokeComp->SetCollisionProfileName(TEXT("OverlapAll"));
	SmokeComp->SetWorldScale3D(FVector(.5f, 0.5f, 0.5f));

	//Create Ammo Text
	AmmoLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SmokeLabel"));
	AmmoLabel->SetupAttachment(SmokeComp);
	AmmoLabel->SetText(TEXT("Smoke"));
	AmmoLabel->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));
	AmmoLabel->SetRelativeLocation(FVector(0.0f, 0.f, 43.f));
	AmmoLabel->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
}

// Called when the game starts or when spawned
void ASmokeAmmo::BeginPlay()
{
	Super::BeginPlay();
	
	//Call overlap function when the player overlaps the static mesh
	SmokeComp->OnComponentBeginOverlap.AddDynamic(this, &ASmokeAmmo::OnOverlapBegin);

	//get reference
	PlayerRef = Cast<AEnemyShooterCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

// Called every frame
void ASmokeAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerRef)
	{
		//how much the Text needs to rotate before facing the player
		FRotator SetLocation_Rot = UKismetMathLibrary::FindLookAtRotation(AmmoLabel->GetComponentLocation(), PlayerRef->GetActorLocation());

		//Only Rotate on the Z Axis
		AmmoLabel->SetWorldRotation(FRotator(AmmoLabel->GetRelativeRotation().Pitch, SetLocation_Rot.Yaw, AmmoLabel->GetRelativeRotation().Roll));
	}
}

void ASmokeAmmo::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (PlayerRef == OtherActor)
	{
		//If player is currently using the smoke ammo then update the current ammo selected
		if (PlayerRef->ammotype == AmmoType::Ice)
		{
			FString tester = TEXT("tester");
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, tester);

			//If Smoke Ammo is below DefaultSmokeAmmo add  SmokeAmmo but clamp the SmokeAmmo
			//so the bullets added does not go over the max SmokeAmmo limit
			//then destroy itself
			if (PlayerRef->Ammo < PlayerRef->DefaultAmmo)
			{
				PlayerRef->Ammo = FMath::Clamp(PlayerRef->Ammo + PlayerRef->DefaultClip, 0, PlayerRef->DefaultAmmo);

				//if the magazine clip is empty then add ammo to the magazine clip
				if (PlayerRef->Clip == 0)
				{
					PlayerRef->Clip = PlayerRef->DefaultClip;
					PlayerRef->Ammo -= PlayerRef->DefaultClip;
				}
				Destroy();
			}
			else
			{
				FString message = TEXT("Ammo Full");
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, message);
			}
		}
		else//if the player has not got the smoke ammo selected then add to lastSmokeAmmo variable instead.
		{
			//If Smoke Ammo is below DefaultSmokeAmmo add  SmokeAmmo but clamp the SmokeAmmo
			//so the bullets added does not go over the max SmokeAmmo limit
			//then destroy itself
			if (PlayerRef->lastIceAmmo < PlayerRef->DefaultIceAmmo)
			{
				PlayerRef->lastIceAmmo = FMath::Clamp(PlayerRef->lastIceAmmo + PlayerRef->DefaultIceClip, 0, PlayerRef->DefaultIceAmmo);

				//if the magazine clip is empty then add ammo to the magazine clip
				if (PlayerRef->lastIceClip == 0)
				{
					PlayerRef->lastIceClip = PlayerRef->DefaultIceClip;
					PlayerRef->lastIceAmmo -= PlayerRef->DefaultIceClip;
				}
				Destroy();
			}
			else
			{
				FString message = TEXT("Ammo Full");
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, message);
			}
		}
	}
	else
	{
		FString message = TEXT("Ref failed");
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, message);
	}
}