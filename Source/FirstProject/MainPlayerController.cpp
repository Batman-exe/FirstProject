// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (WStartScreenMenu)
	{
		StartScreenMenu = CreateWidget<UUserWidget>(this, WStartScreenMenu);
		StartScreenMenu->AddToViewport();
		StartScreenMenu->SetVisibility(ESlateVisibility::Visible);

		FInputModeUIOnly InputUIOnly;
		SetInputMode(InputUIOnly);
		bShowMouseCursor = true;
	}

	if (HUDOverlayAsset)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
		HUDOverlay->AddToViewport();
		HUDOverlay->SetVisibility(ESlateVisibility::Hidden);
	}

	if (WEnemyHealthBar)
	{
		EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
		if (EnemyHealthBar)
		{
			EnemyHealthBar->AddToViewport();
			EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
		}
		FVector2D Alignment(0.f, 0.f);
		EnemyHealthBar->SetAlignmentInViewport(Alignment);
	}
	
	if (WPauseMenu)
	{
		PauseMenu = CreateWidget<UUserWidget>(this, WPauseMenu);
		if (PauseMenu)
		{
			PauseMenu->AddToViewport();
			PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	
	if (WGameOver)
	{
		GameOver = CreateWidget<UUserWidget>(this, WGameOver);
		if (GameOver)
		{
			GameOver->AddToViewport();
			GameOver->SetVisibility(ESlateVisibility::Hidden);
			bGameOverVisible = false;
		}
	}

	if (WVictoryScreen)
	{
		VictoryScreen = CreateWidget<UUserWidget>(this, WVictoryScreen);
		if (VictoryScreen)
		{
			VictoryScreen->AddToViewport();
			VictoryScreen->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyHealthBar)
	{
		FVector2D PositionInViewport;
		ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport);
		PositionInViewport.Y -= 85.f;

		FVector2D SizeInViewport(300.f, 25.f);

		EnemyHealthBar->SetPositionInViewport(PositionInViewport);
		EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
	}
}

void AMainPlayerController::SetHUDOverlayVisibility(bool Visible)
{
	if (HUDOverlay)
	{
		if(Visible)
			HUDOverlay->SetVisibility(ESlateVisibility::Visible);
		else
			HUDOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::DisplayEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = true;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemoveEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = false;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::DisplayPauseMenu_Implementation()
{
	if (PauseMenu)
	{
		bPauseMenuVisible = true;
		PauseMenu->SetVisibility(ESlateVisibility::Visible);

		SetHUDOverlayVisibility(false);

		FInputModeGameAndUI InputModelGameAndUI;
		SetInputMode(InputModelGameAndUI);
		bShowMouseCursor = true;
	}
}

void AMainPlayerController::RemovePauseMenu_Implementation()
{
	if (PauseMenu)
	{
		GameModeOnly();

		bShowMouseCursor = false;

		bPauseMenuVisible = false;

		SetHUDOverlayVisibility(true);
	}
}

void AMainPlayerController::TogglePauseMenu()
{
	bPauseMenuVisible ? RemovePauseMenu() : DisplayPauseMenu();
}

void AMainPlayerController::DisplayGameOver_Implementation()
{
	if (GameOver)
	{
		bGameOverVisible = true;
		GameOver->SetVisibility(ESlateVisibility::Visible);

		SetHUDOverlayVisibility(false);

		FInputModeGameAndUI InputModelGameAndUI;
		SetInputMode(InputModelGameAndUI);
		bShowMouseCursor = true;
	}
}

void AMainPlayerController::DisplayVictoryScreen_Implementation()
{
	VictoryScreen->SetVisibility(ESlateVisibility::Visible);
	SetHUDOverlayVisibility(false);
	FInputModeGameAndUI InputModelGameAndUI;
	SetInputMode(InputModelGameAndUI);
}

void AMainPlayerController::GameModeOnly()
{
	FInputModeGameOnly InputModelGameOnly;
	SetInputMode(InputModelGameOnly);
}
