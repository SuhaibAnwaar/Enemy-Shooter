// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnemyShooterGameMode.generated.h"

class AEnemyShooterCharacter;

UCLASS(minimalapi)
class AEnemyShooterGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AEnemyShooterGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	AEnemyShooterCharacter* PlayerRef;
};



