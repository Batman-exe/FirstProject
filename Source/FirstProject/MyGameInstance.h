// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMyGameInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveGame")
	class UFirstSaveGame* SaveGameObject;

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void UpdateSaveGameObject(FString SlotName, UFirstSaveGame* FirstSaveGame);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	FString GetCurrentSlot() const;

	UFUNCTION(BlueprintCallable)
	void SetChangingLevelInVolume(bool NewValue);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetChangingLevelInVolume() const { return bChangingLevelInVolume; }

	UPROPERTY(BlueprintReadWrite)
	FString CurrentLevelName;

protected:
	FString CurrentSlot;
	
	bool bChangingLevelInVolume;
};
