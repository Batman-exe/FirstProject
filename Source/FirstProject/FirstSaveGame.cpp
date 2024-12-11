// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstSaveGame.h"

UFirstSaveGame::UFirstSaveGame()
{
	SlotName = TEXT("Default");
	UserIndex = 0;

	CharacterStats.Coins = 0;
	CharacterStats.Health = 65.f;
	CharacterStats.MaxHealth = 100.f;
	CharacterStats.Stamina = 120.f;
	CharacterStats.MaxStamina = 150.f;
	CharacterStats.WeaponName = TEXT("");
	CharacterStats.LevelName = TEXT("");

	// initialized in -1 to know that this SaveGameObject hasn't been used
	SunTempleAliveEnemies = { -1, -1, -1, -1, -1, -1 };
}

TArray<int32> UFirstSaveGame::GetCurrentLevelAliveEnemies(FString LevelName) 
{
	if (LevelName == "SunTemple")
		return SunTempleAliveEnemies;
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Level not found at UFirstSaveGame::GetCurrentLevelAliveEnemies()"));
		return TArray<int32>();
	}
}

void UFirstSaveGame::SetCurrentLevelAliveEnemiesAtIndex(FString LevelName, int32 VolumeIndex, int32 Value)
{
	if (LevelName == "SunTemple")
		SunTempleAliveEnemies[VolumeIndex] = Value;
	else
		UE_LOG(LogTemp, Error, TEXT("Level not found at UFirstSavegame()::SetCurrentLevelAliveEnemiesAtIndex()"));
}
