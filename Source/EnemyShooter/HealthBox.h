// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthBox.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UTextRenderComponent;
class AEnemyShooterCharacter;

UCLASS()
class ENEMYSHOOTER_API AHealthBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHealthBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	//Static mesh that represents the Health box
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* HealthComp;

	//The text that displays Health
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTextRenderComponent* HealthLabel;

	//Player Reference
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		AEnemyShooterCharacter* PlayerRef;

	//functions
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
