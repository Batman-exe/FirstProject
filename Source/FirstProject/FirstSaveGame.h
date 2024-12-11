// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FirstSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Health;
	
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxHealth;
	
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Stamina;
	
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxStamina;
	
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int32 Coins;
	
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FVector Location;
	
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FRotator Rotation;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FString WeaponName;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FString LevelName;
};

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API UFirstSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UFirstSaveGame();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Basic")
	FString SlotName;

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	uint32 UserIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Basic")
	FCharacterStats CharacterStats;

	/**
	 * TArray that will storage the amount of enemies alive in the map with the same name 
	 * number n at pos i in array represents n enemies in the EnemiesSpawnVolumne with i id
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Level")
	TArray<int32> SunTempleAliveEnemies;

	UFUNCTION(BlueprintCallable, Category = "Level")
	TArray<int32> GetCurrentLevelAliveEnemies(FString LevelName);

	/*
	 * Sets Value in CurrentLevelAliveEnemies[VolumeIndex]
	 */
	UFUNCTION(BlueprintCallable, Category = "Level")
	void SetCurrentLevelAliveEnemiesAtIndex(FString LevelName, int32 VolumeIndex, int32 Value);
};
