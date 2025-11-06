// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "EnemyShooterCharacter.h"
#include "Turret.generated.h"

UCLASS()
class ENEMYSHOOTER_API ATurret : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurret();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Components
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Cube;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Cone;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UWidgetComponent* FiringPoint;

	//Variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool Shooting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class AEnemyShooterCharacter* PlayerRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> ActorToSpawn;

	//Functions
	void ShootTarget();
};
