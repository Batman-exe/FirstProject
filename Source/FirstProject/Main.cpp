// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"
#include "MainPlayerController.h"
#include "FirstSaveGame.h"
#include "ItemStorage.h"
#include "MyGameInstance.h"
#include "Engine/SkeletalMeshSocket.h"
#include "MovableObject.h"

// Sets default values
AMain::AMain()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create camera boom, pulls towards the player if there's a collision
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f; //Camera follows at this distance
	CameraBoom->bUsePawnControlRotation = true; // Rotate arm based on controller

	// Set size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(37.99f, 104.0f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation 
	FollowCamera->bUsePawnControlRotation = false;

	// Set our turn rates for input
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	// Makes character not rotate with the camera
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// Configure character Movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character faces the direction of input ...
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // ... at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 650.f;
	GetCharacterMovement()->AirControl = 0.2f;

	MaxHealth = 100.f;
	Health = 65.f;
	MaxStamina = 150.f;
	Stamina = 120.f;
	Coins = 0;

	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;

	bShiftKeyDown = false;
	bLMBDown = false;
	bESCDown = false;

	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	StaminaDrainrate = 25.f;
	MinSprintStamina = 50.f;

	InterpSpeed = 15.f;
	bInterpToEnemy = false;

	bHasCombatTarget = false;

	bMovingForward = false;
	bMovingRight = false;
	bIsInAir = false;
	bIsRolling = false;
	bIsGrabbing = false;

	MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this));
	SaveGameObject = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	//MyGameInstance->UpdateSaveGameObject("Slot0", SaveGameObject);

	CurrentAttackAnim = 1;

	bIsPlayingCinematic = false;
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();

	MainPlayerController = Cast<AMainPlayerController>(GetController());

	FString Map = GetWorld()->GetMapName();
	Map.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	if (Map != "StartScreen" && MainPlayerController)
	{
		MainPlayerController->GameModeOnly();
		if(Map != "ElvenRuins")
			MainPlayerController->SetHUDOverlayVisibility(true);
		CurrentLevelName = FName(Map);
	}

	if (!MyGameInstance)
		UE_LOG(LogTemp, Warning, TEXT("GameInstance not found in AMain"));
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	float DeltaStamina = StaminaDrainrate * DeltaTime;

	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if (bShiftKeyDown && (bMovingForward || bMovingRight) && !bIsInAir)
		{
			if (Stamina - DeltaStamina <= MinSprintStamina)
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
			Stamina -= DeltaStamina;

			if (bMovingForward || bMovingRight)
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
		}
		else
		{
			if (Stamina + DeltaStamina >= MaxStamina)
				Stamina = MaxStamina;
			else
				Stamina += DeltaStamina;
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_BelowMinimum:
		if (bShiftKeyDown && (bMovingForward || bMovingRight) && !bIsInAir)
		{
			if (Stamina - DeltaStamina <= 0.f)
			{
				Stamina = 0;
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else
			{
				Stamina -= DeltaStamina;
				if (bMovingForward || bMovingRight)
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}

		}
		else
		{
			if (Stamina - DeltaStamina >= MinSprintStamina)
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
			Stamina += DeltaStamina;
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_Exhausted:
		if (bShiftKeyDown && (bMovingForward || bMovingRight) && !bIsInAir)
			Stamina = 0.f;
		else
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	case EStaminaStatus::ESS_ExhaustedRecovering:
		if (Stamina + DeltaStamina >= MinSprintStamina)
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
		Stamina += DeltaStamina;
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	default:
		;
	}

	if (bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);
		SetActorRotation(InterpRotation);
	}

	if (CombatTarget)
	{
		CombatTargetLocation = CombatTarget->GetActorLocation();
		if (MainPlayerController)
		{
			MainPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}

	bIsInAir = Super::GetMovementComponent()->IsFalling();
}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMain::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUp);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMain::LMBUp);
	
	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMain::ESCDown);
	PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMain::ESCUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AMain::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMain::LookUp);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpAtRate);

}

void AMain::MoveForward(float Value)
{
	bMovingForward = false;
	CurrentMoveForwardScale = Value;
	if (CanMove(Value))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
		bMovingForward = true;
	}
}

void AMain::MoveRight(float Value)
{
	bMovingRight = false;
	CurrentMoveRightScale = Value;
	if (CanMove(Value))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
		bMovingRight = true;
	}
}

void AMain::Turn(float Value)
{
	if (CanMoveCamera(Value))
	{
		AddControllerYawInput(Value);
	}
}

void AMain::LookUp(float Value)
{
	if (CanMoveCamera(Value))
	{
		AddControllerPitchInput(Value);
	}
}

bool AMain::CanMove(float Value)
{
	bool ans = false;
	if (MainPlayerController)
		ans = (Value != 0.0f && !bAttacking && MovementStatus != EMovementStatus::EMS_Dead && !MainPlayerController->bPauseMenuVisible && !bIsRolling &&!bIsGrabbing &&!bIsPlayingCinematic );

	return ans;
}

bool AMain::CanMoveCamera(float Value)
{
	bool ans = false;
	if (MainPlayerController)
		ans = (Value != 0.0f && MovementStatus != EMovementStatus::EMS_Dead && !MainPlayerController->bPauseMenuVisible);

	return ans;
}

void AMain::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());

}

void AMain::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LMBDown()
{
	bLMBDown = true;
	
	if (MovementStatus != EMovementStatus::EMS_Dead && !bIsRolling)
	{
		if (MainPlayerController)
		{
			if (!MainPlayerController->bPauseMenuVisible && !bIsPlayingCinematic)
			{
				if (ActiveOverlappingItem)
				{
					AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
					if (Weapon)
					{
						Weapon->Equip(this);
						SetActiveOverlappingItem(nullptr);
					}
				}
				else if (EquippedWeapon)
					Attack(CurrentAttackAnim);
			}
		}
		
	}
}

void AMain::LMBUp()
{
	bLMBDown = false;
}

void AMain::ESCDown()
{
	bESCDown = true;
	if (MainPlayerController) 
	{
		if(!bIsPlayingCinematic)
		MainPlayerController->TogglePauseMenu();
	}
}


void AMain::ESCUp()
{
	bESCDown = false;
}

void AMain::IncrementCoins(int32 Amount)
{
	Coins += 1;
}

void AMain::DecrementHealth(float Amount)
{
	Health -= Amount;
	if (Health <= 0.f)
		Die();
}

void AMain::IncrementHealth(float Amount)
{
	if (Health > 0.f)
	{
		float increasedHealth = Amount + Health;
		if (increasedHealth <= MaxHealth)
			Health += Amount;
		else
			Health = MaxHealth;
	}
}

void AMain::Die()
{
	if (MovementStatus != EMovementStatus::EMS_Dead)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			AnimInstance->Montage_Play(CombatMontage, 1.0f);
			AnimInstance->Montage_JumpToSection(FName("Death"));
		}
		SetMovementStatus(EMovementStatus::EMS_Dead);
	}
}

void AMain::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
	if (MainPlayerController)
		MainPlayerController->DisplayGameOver();
}

void AMain::UpdateCombatTarget()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	if (OverlappingActors.Num() > 0)
	{
		AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);
		if (ClosestEnemy)
		{
			FVector Location = GetActorLocation();
			float MinDistance = (ClosestEnemy->GetActorLocation() - Location).Size();
			for (auto Actor : OverlappingActors)
			{
				AEnemy* Enemy = Cast<AEnemy>(Actor);
				if (Enemy)
				{
					float DistanceToActor = (Enemy->GetActorLocation() - Location).Size();
					if (DistanceToActor < MinDistance)
					{
						MinDistance = DistanceToActor;
						ClosestEnemy = Enemy;
					}
				}
			}
			if (MainPlayerController)
				MainPlayerController->DisplayEnemyHealthBar();
			SetCombatTarget(ClosestEnemy);
			bHasCombatTarget = true;
		}
	}
	else if (OverlappingActors.Num() == 0)
	{
		if (MainPlayerController)
			MainPlayerController->RemoveEnemyHealthBar();
	}
}

void AMain::SaveGame(FString SlotName)
{
	SaveGameObject = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	SaveGameObject->CharacterStats.Health = Health;
	SaveGameObject->CharacterStats.MaxHealth = MaxHealth;
	SaveGameObject->CharacterStats.Stamina = Stamina;
	SaveGameObject->CharacterStats.MaxStamina = MaxStamina;
	SaveGameObject->CharacterStats.Coins = Coins;
	SaveGameObject->CharacterStats.Location = GetActorLocation();
	SaveGameObject->CharacterStats.Rotation = GetActorRotation();
	SaveGameObject->SlotName = SlotName;

	FString MapName = GetWorld()->GetMapName();
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	SaveGameObject->CharacterStats.LevelName = MapName;
	CurrentLevelName = FName(MapName);

	if (EquippedWeapon)
	{
		SaveGameObject->CharacterStats.WeaponName = EquippedWeapon->Name;
	}
	MyGameInstance->UpdateSaveGameObject(SlotName, SaveGameObject);
	UGameplayStatics::SaveGameToSlot(SaveGameObject, SaveGameObject->SlotName, SaveGameObject->UserIndex);
}

void AMain::LoadGame(FString SlotName)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		SaveGameObject = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		if (SaveGameObject)
		{
			MyGameInstance->UpdateSaveGameObject(SlotName, SaveGameObject);
			if (!SaveGameObject->CharacterStats.LevelName.IsEmpty())
			{
				LoadPlayerStats(SlotName, false);
			}
			else
				UE_LOG(LogTemp, Warning, TEXT("LevelName Empty @ AMain::LoadGame()"));
		}
	}
}

void AMain::LoadPlayerStats(FString SlotName, bool LoadPosition)
{
	//SaveGameObject = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	if (SaveGameObject)
	{
		Health = SaveGameObject->CharacterStats.Health;
		MaxHealth = SaveGameObject->CharacterStats.MaxHealth;
		Stamina = SaveGameObject->CharacterStats.Stamina;
		MaxStamina = SaveGameObject->CharacterStats.MaxStamina;
		Coins = SaveGameObject->CharacterStats.Coins;

		if (WeaponStorage)
		{
			AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);

			if (Weapons)
			{
				FString WeaponName = SaveGameObject->CharacterStats.WeaponName;
				if (Weapons->WeaponMap.Contains(WeaponName))
				{
					AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);
					WeaponToEquip->Equip(this);
				}
			}
		}

		if (Health > 0.f)
		{
			SetMovementStatus(EMovementStatus::EMS_Normal);
			GetMesh()->bPauseAnims = false;
			GetMesh()->bNoSkeletonUpdate = false;
		}

		if (MainPlayerController && !bIsPlayingCinematic)
			MainPlayerController->SetHUDOverlayVisibility(true);

		if (LoadPosition)
		{
			SetActorLocation(SaveGameObject->CharacterStats.Location);
			SetActorRotation(SaveGameObject->CharacterStats.Rotation);
		}
	}
	

	UE_LOG(LogTemp, Warning, TEXT("LoadPlayerStats, Level: %s, Coins: %s, Health: %s"), *CurrentLevelName.ToString(), *FString::FromInt(Coins), *FString::SanitizeFloat(Health));
}

void AMain::SwitchLevel(FName LevelName, bool bFromLoad)
{
	MyGameInstance->SetChangingLevelInVolume(!bFromLoad);

	UWorld* World = GetWorld();
	if (World)
	{
		if (LevelName.IsEqual(FName("")))
			LevelName = FName(*SaveGameObject->CharacterStats.LevelName);

		FString CurrentLevel = World->GetMapName();
		CurrentLevelName = FName(CurrentLevel);
		if (CurrentLevelName != LevelName)
		{
			MyGameInstance->CurrentLevelName = CurrentLevel;
			CurrentLevelName = LevelName;
			UGameplayStatics::OpenLevel(World, LevelName);
		}
	}
}

float AMain::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Health -= DamageAmount;
	if (Health <= 0.f)
	{
		Die();
		if (DamageCauser)
		{
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			if (Enemy)
			{
				Enemy->bHasValidTarget = false;
			}
		}
	}

	return DamageAmount;
}

void AMain::SetEquippedWeapon(AWeapon* WeaponToSet)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
	EquippedWeapon = WeaponToSet;
}

void AMain::Attack(int32 AnimationNumber)
{
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead)
	{
		bAttacking = true;
		SetInterpToEnemy(true);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			int32 Selection;
			if (AnimationNumber == 0 || AnimationNumber == 1)
				Selection = AnimationNumber;
			else
				Selection = FMath::RandRange(0, 1);
			switch (Selection)
			{
			case 0:
				AnimInstance->Montage_Play(CombatMontage, 2.8f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
				CurrentAttackAnim = 0;
				break;
			case 1:
				AnimInstance->Montage_Play(CombatMontage, 2.3f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				CurrentAttackAnim = 1;
				break;
			default:
				break;
			}

		}
	}
}

void AMain::Jump()
{
	if (MainPlayerController)
	{
		if (!MainPlayerController->bPauseMenuVisible && !bIsPlayingCinematic)
		{
			if (MovementStatus != EMovementStatus::EMS_Dead && !bIsGrabbing)
			{
				Super::Jump();
			}
		}
	}
}

void AMain::AttackEnd()
{
	bAttacking = false;
	SetInterpToEnemy(false);
	if (bLMBDown)
	{
		if (CurrentAttackAnim == 0)
			Attack(1);
		else if (CurrentAttackAnim == 1)
			Attack(0);
	}
	else
	{
		if(CurrentAttackAnim == 0)
			CurrentAttackAnim = 1;
		else
			CurrentAttackAnim = 0;
	}
}

void AMain::Roll()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage && MovementStatus != EMovementStatus::EMS_Dead && !bIsPlayingCinematic)
	{
		bIsRolling = true;
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
		AnimInstance->Montage_Play(CombatMontage, 2.0f);
		AnimInstance->Montage_JumpToSection(FName("Roll"));
	}
}

void AMain::UpdateRollPosition(float DeltaPos)
{
	FVector NewPosition = GetActorLocation() + GetActorForwardVector() * DeltaPos;
	SetActorLocation(NewPosition);
}

void AMain::PlaySwingSound()
{
	if (EquippedWeapon->SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
}

FRotator AMain::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);

	return LookAtRotationYaw;
}

void AMain::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	else
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;

}

void AMain::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

void AMain::ShiftKeyDown()
{
	bShiftKeyDown = true;
}

void AMain::ShiftKeyUp()
{
	bShiftKeyDown = false;
}

void AMain::ShowPickupLocations()
{
	for (int32 i = 0; i < PickupLocations.Num(); i++)
		UKismetSystemLibrary::DrawDebugSphere(this, PickupLocations[i], 25.f, 8, FLinearColor::Green, 5.f, 0.25f);

}

bool AMain::IsPlayingCinematic()
{
	return bIsPlayingCinematic;
}

void AMain::SetIsPlayingCinematic(bool IsPlayingCinematic)
{
	bIsPlayingCinematic = IsPlayingCinematic;
}
