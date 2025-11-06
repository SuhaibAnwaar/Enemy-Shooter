// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoBox.h"
#include "AmmoBox.h"
#include "EnemyShooterCharacter.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAmmoBox::AAmmoBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create Static Mesh
	AmmoComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoComp"));
	AmmoComp->SetupAttachment(RootComponent);
	AmmoComp->SetSimulatePhysics(false);
	AmmoComp->SetCollisionProfileName(TEXT("OverlapAll"));
	AmmoComp->SetWorldScale3D(FVector(.5f, 0.5f, 0.5f));

	//Create Ammo Text
	AmmoLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("AmmoLabel"));
	AmmoLabel->SetupAttachment(AmmoComp);
	AmmoLabel->SetText(TEXT("Ammo"));
	AmmoLabel->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));
	AmmoLabel->SetRelativeLocation(FVector(0.0f, 0.f, 43.f));
	AmmoLabel->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
}

// Called when the game starts or when spawned
void AAmmoBox::BeginPlay()
{
	Super::BeginPlay();
	
	//Call overlap function when the player overlaps the static mesh
	AmmoComp->OnComponentBeginOverlap.AddDynamic(this, &AAmmoBox::OnOverlapBegin);

	//get reference
	PlayerRef = Cast<AEnemyShooterCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

// Called every frame
void AAmmoBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerRef)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "tester");

		//how much the Text needs to rotate before facing the player
		FRotator SetLocation_Rot = UKismetMathLibrary::FindLookAtRotation(AmmoLabel->GetComponentLocation(), PlayerRef->GetActorLocation());

		//Only Rotate on the Z Axis
		AmmoLabel->SetWorldRotation(FRotator(AmmoLabel->GetRelativeRotation().Pitch, SetLocation_Rot.Yaw, AmmoLabel->GetRelativeRotation().Roll));
	}
}

void AAmmoBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (PlayerRef == OtherActor)
	{
		//If player is currently using the orginal ammo then update the current ammo selected
		if (PlayerRef->ammotype == AmmoType::Orginal)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "tester");

			//If Ammo is below DefaultAmmo add 20 bullets to Ammo but clamp the Ammo
			//so the bullets added does not go over the max Ammo limit
			//then destroy AmmoBox
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
		else//if the player has not got the orginal ammo selected then add to lastOrginalAmmo variable instead.
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Refilling Orginal ammo");

			//If Orginal Ammo is below DefaultOrginalAmmo add OrginalAmmo but clamp the OrginalAmmo
			//so the bullets added does not go over the max OrginalAmmo limit
			//then destroy itself
			if (PlayerRef->lastOrginalAmmo < PlayerRef->DefaultOrginalAmmo)
			{
				PlayerRef->lastOrginalAmmo = FMath::Clamp(PlayerRef->lastOrginalAmmo + PlayerRef->DefaultOrginalClip, 0, PlayerRef->DefaultOrginalAmmo);

				//if the magazine clip is empty then add ammo to the magazine clip
				if (PlayerRef->lastOrginalClip == 0)
				{
					PlayerRef->lastOrginalClip = PlayerRef->DefaultOrginalClip;
					PlayerRef->lastOrginalAmmo -= PlayerRef->DefaultOrginalClip;
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

