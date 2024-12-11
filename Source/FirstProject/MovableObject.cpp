// Fill out your copyright notice in the Description page of Project Settings.


#include "MovableObject.h"
#include "Main.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

// Sets default values
AMovableObject::AMovableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	StaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	RootComponent = StaticMeshComponent;
	
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	TriggerBox->SetBoxExtent(FVector(62.f, 62.f, 32.f));
	TriggerBox->SetupAttachment(GetRootComponent());


	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("PhysicsConstraint"));
	PhysicsConstraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Free, 0.0f);
	PhysicsConstraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Free, 0.0f);
	PhysicsConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Free, 0.0f);
	PhysicsConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Free, 0.0f);
	PhysicsConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0.0f);
	PhysicsConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0.0f);
	PhysicsConstraint->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AMovableObject::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AMovableObject::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AMovableObject::OnOverlapEnd);
}

// Called every frame
void AMovableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMovableObject::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMain* Main = Cast<AMain>(OtherActor);
	if (Main)
	{
		Main->SetActiveOverlappingMovable(this);
	}
}

void AMovableObject::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMain* Main = Cast<AMain>(OtherActor);
	if (Main)
	{
		Main->SetActiveOverlappingMovable(nullptr);
	}
}

void AMovableObject::SimulatePhysics(bool Simulate)
{
	StaticMeshComponent->SetSimulatePhysics(Simulate);
}

