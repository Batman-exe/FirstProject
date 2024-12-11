// Fill out your copyright notice in the Description page of Project Settings.


#include "Healing.h"
#include "Main.h"
#include "Components/SphereComponent.h"

AHealing::AHealing()
{
	Heal = 10;

	CollisionVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
}

void AHealing::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	UE_LOG(LogTemp, Warning, TEXT("Healing::OnOverlapBegin"));

	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main) 
		{
			Main->IncrementHealth(Heal);
			Main->PickupLocations.Add(GetActorLocation());
			
			Destroy();
		}
	}

}

void AHealing::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	UE_LOG(LogTemp, Warning, TEXT("Healing::OnOverlapEnd"));
}
