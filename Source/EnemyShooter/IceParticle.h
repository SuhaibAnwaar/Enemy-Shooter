// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IceParticle.generated.h"

class UParticleSystemComponent;
class USphereComponent;
class AAICharacter;

UCLASS()
class ENEMYSHOOTER_API AIceParticle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIceParticle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Ice Particle
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UParticleSystemComponent* Ice;

	//Used for overlap events so the smoke can blind the AI
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USphereComponent* SphereCollision;

	//did something overlap the sphere collision
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isOverlap;

	AAICharacter* AIRef;

	//Destroy self
	void Die();

	//Functions 
	// declare Sphere Component overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
