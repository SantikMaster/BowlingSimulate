// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyBowlingGameMode.h"
#include "MyBowlingHUD.h"
#include "MyBowlingCharacter.h"
#include "UObject/ConstructorHelpers.h"



AMyBowlingGameMode::AMyBowlingGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AMyBowlingHUD::StaticClass();
}
