// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KeyComp.generated.h"

class UStaticMeshComponent;
class AEnemyShooterCharacter;
class UTextRenderComponent;

UCLASS()
class ENEMYSHOOTER_API AKeyComp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKeyComp();

	//Reference to player
	AEnemyShooterCharacter* PlayerRef;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* KeyMesh;

	//The text that displays what the ammo type is
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UTextRenderComponent* KeyLabel;

	//functions
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool isOverlapped;
};
