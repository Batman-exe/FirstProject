// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "FirstSaveGame.h"

UMyGameInstance::UMyGameInstance()
{
	SaveGameObject = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	CurrentSlot = "Slot0";
	bChangingLevelInVolume = false;
	CurrentLevelName = "";
}

void UMyGameInstance::UpdateSaveGameObject(FString SlotName, UFirstSaveGame* FirstSaveGame)
{
	CurrentSlot = SlotName;
	SaveGameObject = FirstSaveGame;
	CurrentLevelName = SaveGameObject->CharacterStats.LevelName;
}

FString UMyGameInstance::GetCurrentSlot() const
{
	return CurrentSlot;
}

void UMyGameInstance::SetChangingLevelInVolume(bool NewValue)
{
	bChangingLevelInVolume = NewValue;
}
