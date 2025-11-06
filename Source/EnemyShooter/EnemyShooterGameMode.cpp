// Copyright Epic Games, Inc. All Rights Reserved.

#include "EnemyShooterGameMode.h"
#include "EnemyShooterHUD.h"
#include "EnemyShooterCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

AEnemyShooterGameMode::AEnemyShooterGameMode()
	: Super()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/Player/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AEnemyShooterHUD::StaticClass();
} 

void AEnemyShooterGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	//get reference to player
	PlayerRef = Cast<AEnemyShooterCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}
void AEnemyShooterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerRef)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Cast success");
		if (PlayerRef->isDead)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, "Working");
		
			//Restart Level when player dies
			UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);

		}
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Cast fail");
	}

}
