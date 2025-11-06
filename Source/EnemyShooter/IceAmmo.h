// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IceAmmo.generated.h"

class UStaticMeshComponent;
class UTextRenderComponent;
class AEnemyShooterCharacter;

UCLASS()
class ENEMYSHOOTER_API AIceAmmo : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIceAmmo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Static mesh that represents the Ammo
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* IceComp;

	//The text that displays what the ammo type is
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UTextRenderComponent* AmmoLabel;

	//Player Reference
	AEnemyShooterCharacter* PlayerRef;

	//functions
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
