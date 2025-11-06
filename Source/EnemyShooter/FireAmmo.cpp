// Fill out your copyright notice in the Description page of Project Settings.


#include "FireAmmo.h"
#include "EnemyShooterCharacter.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFireAmmo::AFireAmmo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//Create Static Mesh
	FireComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoComp"));
	FireComp->SetupAttachment(RootComponent);
	FireComp->SetSimulatePhysics(false);
	FireComp->SetCollisionProfileName(TEXT("OverlapAll"));
	FireComp->SetWorldScale3D(FVector(.5f, 0.5f, 0.5f));

	//Create Ammo Text
	AmmoLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("FireLabel"));
	AmmoLabel->SetupAttachment(FireComp);
	AmmoLabel->SetText(TEXT("Fire"));
	AmmoLabel->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));
	AmmoLabel->SetRelativeLocation(FVector(0.0f, 0.f, 43.f));
	AmmoLabel->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
}

// Called when the game starts or when spawned
void AFireAmmo::BeginPlay()
{
	Super::BeginPlay();

	//Call overlap function when the player overlaps the static mesh
	FireComp->OnComponentBeginOverlap.AddDynamic(this, &AFireAmmo::OnOverlapBegin);

	//get reference
	PlayerRef = Cast<AEnemyShooterCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

// Called every frame
void AFireAmmo::Tick(float DeltaTime)
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

void AFireAmmo::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (PlayerRef == OtherActor)
	{
		//If player is currently using the fire ammo then update the current ammo selected
		if (PlayerRef->ammotype == AmmoType::Fire)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "tester");

			//If Ammo is below DefaultFireAmmo add 10 bullets to FireAmmo but clamp the FireAmmo
			//so the bullets added does not go over the max FireAmmo limit
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
		else//if the player has not got the fire ammo selected then add lastFireAmmo variable instead.
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Refilling Fire ammo");

			//If Fire Ammo is below DefaultFireAmmo add more FireAmmo but clamp the FireAmmo
			//so the bullets added does not go over the max FireAmmo limit
			//then destroy itself
			if (PlayerRef->lastFireAmmo < PlayerRef->DefaultFireAmmo)
			{
				PlayerRef->lastFireAmmo = FMath::Clamp(PlayerRef->lastFireAmmo + PlayerRef->DefaultFireClip, 0, PlayerRef->DefaultFireAmmo);

				//if the magazine clip is empty then add ammo to the magazine clip
				if (PlayerRef->lastFireClip == 0)
				{
					PlayerRef->lastFireClip = PlayerRef->DefaultFireClip;
					PlayerRef->lastFireAmmo -= PlayerRef->DefaultFireClip;
				}
				Destroy();
			}
			else
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Ammo Full");
			}
		}
	}
	else
	{
		FString message = TEXT("Ref failed");
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, message);
	}
}