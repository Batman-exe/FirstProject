// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Enemy.h"
#include "AIController.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetWorldLocation(Root->GetComponentLocation());

	DoorsContainer = CreateDefaultSubobject<USceneComponent>(TEXT("DoorsContainer"));
	DoorsContainer->SetWorldLocation(Root->GetComponentLocation()+ FVector(0.f,200.f,0.f));
	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->AttachTo(DoorsContainer);

	bSpawnBoss = false;
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	if (Actor_1 && Actor_2 && Actor_3 && Actor_4)
	{
		SpawnArray.Add(Actor_1);
		SpawnArray.Add(Actor_2);
		SpawnArray.Add(Actor_3);
		SpawnArray.Add(Actor_4);
	}

	/*if (bSpawnBoss)
	{
		SpawnOurActor_Implementation(Boss, GetSpawnPoint());
		AliveEnemies++;
	}
	*/
	InitialDoorsLocation = DoorsContainer->GetComponentLocation();
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASpawnVolume::GetSpawnPoint()
{
	FVector Extent = SpawningBox->GetScaledBoxExtent();
	FVector Origin = SpawningBox->GetComponentLocation();

	FVector Point = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);

	return Point;
}

TSubclassOf<AActor> ASpawnVolume::GetSpawnActor()
{
	TSubclassOf<AActor> actor = nullptr;

	if (SpawnArray.Num() > 0)
	{
		int32 index = FMath::RandRange(0, SpawnArray.Num() - 1);

		actor = SpawnArray[index];
	}

	return actor;
}

int32 ASpawnVolume::GetAliveEnemies()
{
	int32 count = 0;
	for (int i = 0; i< SpawnedEnemies.Num(); i++)
	{
		if (SpawnedEnemies[i]->Health > 0.0f)
			count++;
	}
	AliveEnemies = count;
	return AliveEnemies;
}

void ASpawnVolume::DecreaseAliveEnemies()
{
	AliveEnemies--;
	if (AliveEnemies < 1)
	{
		Door->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OpenDoors();
	}
}

void ASpawnVolume::IncreaseAliveEnemies(int32 Amount)
{
	AliveEnemies += Amount;
}

void ASpawnVolume::UpdateDoorsLocation(float Z)
{
	FVector NewLocation = InitialDoorsLocation;
	NewLocation.Z += Z;
	DoorsContainer->SetWorldLocation(NewLocation);
}

void ASpawnVolume::SpawnOurActor_Implementation(UClass* ToSpawn, const FVector& Location)
{
	if (ToSpawn)
	{
		UWorld* World = GetWorld();
		FActorSpawnParameters SpawnParams;

		if (World)
		{
			AActor * Actor = World->SpawnActor<AActor>(ToSpawn, Location, FRotator(0.f), SpawnParams);
			
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			
			if (Enemy)
			{
				Enemy->SetSpawnVolume(this);

				SpawnedEnemies.Add(Enemy);

				Enemy->SpawnDefaultController();
				
				AAIController* AICont = Cast<AAIController>(Enemy->GetController());
				
				if (AICont)
				{
					Enemy->AIController = AICont;
				}
			}
		}
	}
}
