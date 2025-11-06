// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyShooterCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;
class UWidgetComponent;
class USceneCaptureComponent2D;
class USpringArmComponent;
class UPawnSensingComponent;
class UPawnNoiseEmitterComponent;
class UBoxComponent;
class AKeyComp;
class APlayerCheckpoint;
class UPlayerGameInstance;

UENUM(BlueprintType)
enum class AmmoType : uint8
{
	Orginal UMETA(DisplayName = "Orginal"),
	Ice UMETA(DisplayName = "Ice"),
	Fire UMETA(DisplayName = "Fire"),
};

UCLASS(config=Game)
class AEnemyShooterCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* VR_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	//Spring arm to hold minimap Scene capture
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* SpringArm;

	//Minimap Scene capture
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneCaptureComponent2D* Minimap;

public:
	AEnemyShooterCharacter();

	//Components
	//A plane which shows which way the player is facing on the minimap
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* Arrowicon;

	//Emits the sounds to nearby AI
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UPawnNoiseEmitterComponent* PawnNoiseEmitterComp;

	//if the Enemy is inside the collision box then the player will be able to see their health bars
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UBoxComponent* CanSeeEnemyCollision;

protected:
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Projectile)
	TSubclassOf<class AActor> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	//the seconds to wait between shots
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float TimeBetweenShots;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint8 bUsingMotionControllers : 1;

	//Is Player Dead
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		bool isDead;

	//Inital Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float DefaultHealth;

	//Current Health
	UPROPERTY(BlueprintReadOnly, Category = "Health")
		float Health;

	//did players health increase
	UPROPERTY(BlueprintReadOnly, Category = "Health")
		bool HealthIncrease;
	
	//Timer Handle for firing a projectile
	FTimerHandle TH_Fire;
		
	//Max Ammo size of current Ammo Type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int DefaultAmmo;

	//Current Ammo size of current Ammo Type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int Ammo;

	//Max bullets per Clip for current Ammo Type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
		int DefaultClip;

	//Current bullets in Clip for Current Ammo type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
		int Clip;

	//enum holds all the ammo types
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
		TEnumAsByte<AmmoType> ammotype;
	
	//Find all instances of the enemy
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo")
		TSubclassOf<AActor> ClassToFind;

	//the recharge time for each shot of the fire ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
		float FireRecharge;

	//the recharge time for each shot of the smoke ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
		float IceRecharge;

	//Max Ammo size for each Ammo Type
	int DefaultOrginalAmmo;
	int DefaultIceAmmo;
	int DefaultFireAmmo;
	
	//Max Clip Size for each Ammo Type
	int DefaultOrginalClip;
	int DefaultIceClip;
	int DefaultFireClip;
	
	//The last time the gun can be reloaded
	int LastReload;

	//When the player can reload
	int StartClipReload;

	//the amount of ammo left when swicthing to a different ammo type
	int lastOrginalAmmo;
	int lastIceAmmo;
	int lastFireAmmo;

	//the amount of bullets left in the clip when swicthing to a different ammo type
	int lastOrginalClip;
	int lastIceClip;
	int lastFireClip;

	//checks if player shot a ice bullet
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	bool IceShot;
	
	//tells the projectile the ice bullet has been shot
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	bool isIceShot;

	//checks if player shot a fire bullet
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	bool FireShot;

	//tells the projectile the fire bullet has been shot
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	bool isFireShot;

	//Timer Handle for the recharge time
	FTimerHandle RechargeFireHandle;
	FTimerHandle RechargeIceHandle;

	//all the keys the player owns
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
		TArray<AKeyComp*> KeyCompVariables;

	//the checkpoint where the player will respawn to 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
		APlayerCheckpoint* RespawnCheckpoint;

	//Reference to the Game Instance class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
		UPlayerGameInstance* GameInstanceRef;

	/** Sound to play each time we get shot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		USoundBase* TakeDamageSound;

protected:

	//Projectile Functions ///////////////////
	//Reloads Weapon
	void Reload();

	/** Player presses down on the fire button */
	void StartFire();

	/** Player releases the fire button */
	void StopFire();

	/** Fires a projectile. */
	void OnFire();
	/////////////////////////////////////////

	//Receives Damage
	UFUNCTION()
		virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	//Change Ammo Type
	void AmmoSelector();

	//Reset SmokeShot Boolean
	void RestSmokeShot();

	//Reset FireShot Boolean
	void RestFireShot();
	
	//Recharge fire bullet
	void RechargeFireBullet();	

	//Recharge smoke bullet
	void RechargeSmokeBullet();

	//Play Sound and Report it
	UFUNCTION(BlueprintCallable, Category = AI)
		void ReportNoise(USoundBase* SoundToPlay, float Volume);

	//On Button Press the player will be able to interact with objects
	void Interact();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface


public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

