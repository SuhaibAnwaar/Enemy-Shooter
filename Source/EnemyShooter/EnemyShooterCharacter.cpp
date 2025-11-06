// Copyright Epic Games, Inc. All Rights Reserved.

#include "EnemyShooterCharacter.h"
#include "EnemyShooterProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/SpringArmComponent.h"
#include "AICharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Door.h"
#include "PlayerGameInstance.h"
#include "PlayerCheckpoint.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AEnemyShooterCharacter

AEnemyShooterCharacter::AEnemyShooterCharacter()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	
	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.
	
	//create spring arm for minimap Scene capture
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeRotation(FRotator(-90.f, 180.f, 180.f));
	SpringArm->TargetArmLength = 450.f;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;

	//Create Scene Capture component to be used as the minimap camera
	Minimap = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Minimap"));
	Minimap->SetupAttachment(SpringArm);
	Minimap->ProjectionType = ECameraProjectionMode::Orthographic;
	Minimap->OrthoWidth = 2000.f;

	//Create static mesh to be used as a plane
	Arrowicon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arrowicon"));
	Arrowicon->SetupAttachment(RootComponent);
	Arrowicon->SetRelativeLocation(FVector(0.f, 0.f, 93.f));
	Arrowicon->SetRelativeRotation(FRotator(0.0f, 90.f, 0.f));
	Arrowicon->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));
	Arrowicon->CastShadow = false;

	//so the player cannot see the their own arrow
	Arrowicon->bOwnerNoSee = true;

	//Create Noise Emitter
	PawnNoiseEmitterComp = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("PawnNoiseEmitter"));

	//Create Box Collision 
	CanSeeEnemyCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CanSeeEnemyCollision"));
	CanSeeEnemyCollision->SetupAttachment(RootComponent);
	CanSeeEnemyCollision->SetBoxExtent(FVector(600.f, 300.f, 32.f));
	CanSeeEnemyCollision->SetCollisionProfileName(TEXT("Custom"));

	//Set inital ammo type
	ammotype = AmmoType::Orginal;	

	//Health
	isDead = false;
	DefaultHealth = 100.f;
	Health = DefaultHealth;
	HealthIncrease = false;
	
	//Ammo
	LastReload = 20;
	StartClipReload = 20;

	//Orginal ammo
	DefaultOrginalAmmo = 100;
	DefaultOrginalClip = 20;

	//Ice ammo
	DefaultIceAmmo = 20;
	DefaultIceClip = 10;
	IceShot = false;
	isIceShot = false;
	IceRecharge = 10.f;

	//Fire ammo
	DefaultFireAmmo = 20;
	DefaultFireClip = 10;
	FireShot = false;
	isFireShot = false;
	FireRecharge = 5.f;

	//Starting ammo and clip size
	DefaultAmmo = DefaultOrginalAmmo;
	Ammo = DefaultAmmo;
	DefaultClip = DefaultOrginalClip;
	Clip = DefaultClip;
}

void AEnemyShooterCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	//get reference to game instance class
	GameInstanceRef = Cast<UPlayerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	//we do not want the player respawning outside the map or in the wrong position
	if (GameInstanceRef->PlayerSpawnLocation != FVector(0.0f, 0.0f, 0.0f))
	{
		//set players location to the one saved in the game instance 
		SetActorLocation(GameInstanceRef->PlayerSpawnLocation);
	}

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	//Hide Mesh from the mini map
	Minimap->HideComponent(Mesh1P);
	Minimap->HideComponent(FP_Gun);

	//Get All Enemey characters from the level
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ClassToFind, FoundActors);

	for (int i = 0; i < FoundActors.Num(); i++)
	{
		AAICharacter* AIRef = Cast<AAICharacter>(FoundActors[i]);
		
		if (AIRef)
		{
			//UE_LOG(LogTemp, Warning, TEXT("AIRef true"));
			
			//Hide All the Enemy's meshes and guns from the minimap
			Minimap->HideComponent(AIRef->GetMesh());
			Minimap->HideComponent(AIRef->AI_Gun);
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("AIRef false"));
		}
	}
	

	//Set Ammo Values
	lastOrginalAmmo = 100;
	lastOrginalClip = 20;

	lastIceAmmo = 20;
	lastIceClip = 10;

	lastFireAmmo = 20;
	lastFireClip = 10;
	
	LastReload = 20;
	StartClipReload = 20;
	
	//Change the ammo and clip values to the orginal values
	Ammo = lastOrginalAmmo;
	Clip = lastOrginalClip;	
	
}

void AEnemyShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Convert float variable to string so player can see how much ammo it has
	FString messageAmmo = FString::SanitizeFloat(Ammo);
	//GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Blue, *messageAmmo);

	//Convert float variable to string so player can see how many bullets are in the Clip 
	FString messageClip = FString::SanitizeFloat(Clip);
	//GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Blue, *messageClip);

	//check if the player died 
	if (isDead)
	{
		//check if both variables are valid
		if (GameInstanceRef)
		{
			if (RespawnCheckpoint)
			{
				//set the spawn location to the checkpoint location as the player would not want to restart from the begining when they die
				GameInstanceRef->PlayerSpawnLocation = RespawnCheckpoint->GetActorLocation();
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AEnemyShooterCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AEnemyShooterCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AEnemyShooterCharacter::StopFire);

	//Bind Reload event
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AEnemyShooterCharacter::Reload);

	//Bind Switch Ammo type event
	PlayerInputComponent->BindAction("AmmoSelector", IE_Pressed, this, &AEnemyShooterCharacter::AmmoSelector);

	//Bind Interact event
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AEnemyShooterCharacter::Interact);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AEnemyShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AEnemyShooterCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AEnemyShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AEnemyShooterCharacter::LookUpAtRate);

}

void AEnemyShooterCharacter::Reload()
{
	//if there is still ammo left then refill Clip and subtract DefaultClip away from Ammo supply
	if (Ammo >= LastReload)
	{
		if (Clip < StartClipReload)
		{
			Clip = DefaultClip;
			Ammo -= DefaultClip;
		}
		else
		{
			FString message = TEXT("Maagzine Clip Full");
			//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, message);
		}
	}
	else
	{
		FString message = TEXT("No Ammo Left");
		//GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Blue, message);
	}
}

void AEnemyShooterCharacter::StartFire()
{
	if (Clip > 0)
	{
		//check if the smoke grenade was fired
		if (ammotype == AmmoType::Ice)
		{			
			//the ice bullet can only be fired once every 3 seconds
			if (!IceShot)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::White, "Can Shoot Smoke");
				
				IceShot = true;
				isIceShot = true;
				OnFire();

				//IceHandle will reset IceShot boolean after 3 seconds
				FTimerHandle IceHandle;
				GetWorldTimerManager().SetTimer(IceHandle, this, &AEnemyShooterCharacter::RestSmokeShot, 3.f, false);

				IceRecharge = 0.f;

				//RechargeSmokeHandle will call RechargeSmokeBullet to increase SmokeRecharge by .1, this is used for the UI widget 
				GetWorldTimerManager().SetTimer(RechargeIceHandle, this, &AEnemyShooterCharacter::RechargeSmokeBullet, 0.1f, true);
				
				//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "IceFire True");
			}
		}
		else if (ammotype == AmmoType::Fire)
		{
			if (!FireShot)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::White, "Can Shoot Fire");
				
				FireShot = true;
				isFireShot = true;
				OnFire();

				//FireHandle will reset FireShot boolean after 3 seconds
				FTimerHandle FireHandle;
				GetWorldTimerManager().SetTimer(FireHandle, this, &AEnemyShooterCharacter::RestFireShot, 3.f, false);

				FireRecharge = 0.f;

				//RechargeFireHandle will call RechargeFireBullet to increase FireRecharge by .1, this is used for the UI widget 
				GetWorldTimerManager().SetTimer(RechargeFireHandle, this, &AEnemyShooterCharacter::RechargeFireBullet, 0.1f, true);

				//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "FireShot True");
			}
		}
		else if(ammotype == AmmoType::Orginal)
		{	
			//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::White, "Can Shoot Orginal");
			
			//inital shot
			OnFire();

			//the player can hold down the fire button and fire rapdily
			GetWorldTimerManager().SetTimer(TH_Fire, this, &AEnemyShooterCharacter::OnFire, TimeBetweenShots, true);
			
		}
	}
	else
	{
		FString message = TEXT("Reload");
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, message);
		
	}

	if (Clip == 0 && Ammo == 0)
	{
		FString message = TEXT("No Bullets Left");
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, message);
	}
}

void AEnemyShooterCharacter::StopFire()
{
	GetWorldTimerManager().ClearTimer(TH_Fire);
}

void AEnemyShooterCharacter::OnFire()
{
	//Check if Player has no bullets left in Clip
	if (Clip <= 0)
	{
		return;
	}

	// try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<AEnemyShooterProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
			else
			{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
				
				// spawn the projectile at the muzzle
				World->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

				Clip -= 1;
			}
		}
	}

	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		//UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		ReportNoise(FireSound, 1.f);
		
	}

	// try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

float AEnemyShooterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//Reduce Health
	Health = FMath::Clamp(Health - DamageAmount, 0.0f, DefaultHealth);

	//UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health);

	//check sound variable is valid
	if (TakeDamageSound)
	{
		//Play Sound
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), TakeDamageSound, GetActorLocation(), 1);
	}

	//If Health is zero then kill player
	if (Health <= 0.0f)
	{
		isDead = true;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Dead"));
	}

	return DamageAmount;
}

void AEnemyShooterCharacter::AmmoSelector()
{
	if (ammotype == AmmoType::Orginal)
	{
		//UE_LOG(LogTemp, Warning, TEXT("orginal"));
		ammotype = AmmoType::Ice;

		//store the current ammo and bullets in clip for original ammo
		lastOrginalAmmo = Ammo;
		lastOrginalClip = Clip;

		//change the ammo and clip values to the ice values
		Ammo = lastIceAmmo;
		Clip = lastIceClip;
		DefaultAmmo = DefaultIceAmmo;
		DefaultClip = DefaultIceClip;
		
		//change the reload to the ice reload values
		LastReload = DefaultIceClip;
		StartClipReload = DefaultIceClip;
				
	}
	else if (ammotype == AmmoType::Ice)
	{
		//UE_LOG(LogTemp, Warning, TEXT("ice"));
		ammotype = AmmoType::Fire;

		//store the current ammo and bullets in clip for ice ammo
		lastIceAmmo = Ammo;
		lastIceClip = Clip;

		//change the ammo and clip values to the fire values
		Ammo = lastFireAmmo;
		Clip = lastFireClip;
		DefaultAmmo = DefaultFireAmmo;
		DefaultClip = DefaultFireClip;
		
		//change the reload to the fire values
		LastReload = DefaultFireClip;
		StartClipReload = DefaultFireClip;
		
	}
	else if (ammotype == AmmoType::Fire)
	{
		//UE_LOG(LogTemp, Warning, TEXT("fire"));
		ammotype = AmmoType::Orginal;

		//store the current ammo and bullets in clip for fire ammo
		lastFireAmmo = Ammo;
		lastFireClip = Clip;

		//change the ammo and clip values to the orginal values
		Ammo = lastOrginalAmmo;
		Clip = lastOrginalClip;
		DefaultAmmo = DefaultOrginalAmmo;
		DefaultClip = DefaultOrginalClip;
		
		//change the reload to the orginal reload values
		LastReload = DefaultOrginalClip;
		StartClipReload = DefaultOrginalClip;
	}	
}

void AEnemyShooterCharacter::RestSmokeShot()
{
	IceShot = false;
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Smoke Reset");
}

void AEnemyShooterCharacter::RestFireShot()
{
	FireShot = false;
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Fire Reset");
}

void AEnemyShooterCharacter::RechargeFireBullet()
{
	FireRecharge += 0.1;
	//UE_LOG(LogTemp, Warning, TEXT(" %.f"), FireRecharge);

	if (!FireShot)
	{
		GetWorldTimerManager().ClearTimer(RechargeFireHandle);
	}
}

void AEnemyShooterCharacter::RechargeSmokeBullet()
{
	IceRecharge += 0.1;
	//UE_LOG(LogTemp, Warning, TEXT(" %.f"), SmokeRecharge);

	if (!IceShot)
	{
		GetWorldTimerManager().ClearTimer(RechargeIceHandle);
	}
}

void AEnemyShooterCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AEnemyShooterCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AEnemyShooterCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AEnemyShooterCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AEnemyShooterCharacter::ReportNoise(USoundBase* SoundToPlay, float Volume)
{
	//Play Sound and report it
	if (SoundToPlay)
	{
		//Play Sound
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundToPlay, GetActorLocation(), Volume);

		//Report that we have made a noise 
		MakeNoise(Volume, this, GetActorLocation());
	}
}

void AEnemyShooterCharacter::Interact()
{
	//draw line trace from gun's muzzle location
	FVector Loc = FP_MuzzleLocation->GetComponentLocation();
	FRotator Rot = GetControlRotation();
	FHitResult Hit;

	FVector Start = Loc;

	//Line Trace will be 300 units long
	FVector End = Start + (Rot.Vector() * 300);
	FCollisionQueryParams TraceParams;

	//Draw Line trace
	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);

	if (Hit.bBlockingHit)
	{
		//Open Door
		ADoor* DoorRef = Cast<ADoor>(Hit.GetActor());
		if (DoorRef)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "DoorRef True");
			DoorRef->DoorMotion();
		}
	}
	//Draw Line trace so player can see it
	//DrawDebugLine(GetWorld(), Start, End, FColor::Orange, false, 2.0f);
}
