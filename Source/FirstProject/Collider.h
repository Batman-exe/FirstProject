// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Collider.generated.h"

UCLASS()
class FIRSTPROJECT_API ACollider : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACollider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Used for mesh
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UStaticMeshComponent* MeshComponent;

	// Used for collisions
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UCameraComponent* Camera;

	// Distance between Camera and mesh
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	class UColliderMovementComponent* OurMovementComponent;

	virtual UPawnMovementComponent* GetMovementComponent() const override;

	FORCEINLINE UStaticMeshComponent* getMeshComponent() { return MeshComponent; }
	FORCEINLINE void setMeshComponent(UStaticMeshComponent* Mesh) { MeshComponent = Mesh; }
	FORCEINLINE USphereComponent* getSphereComponent() { return SphereComponent; }
	FORCEINLINE void setSphereComponent(USphereComponent* Sphere) { SphereComponent = Sphere; }
	FORCEINLINE UCameraComponent* getCameraComponent() { return Camera; }
	FORCEINLINE void setCameraComponent(UCameraComponent* InCamera) { Camera = InCamera; }
	FORCEINLINE USpringArmComponent* getSpringArmComponent() { return SpringArm; }
	FORCEINLINE void setSpringArmComponent(USpringArmComponent* InSpringArm) { SpringArm = InSpringArm; }

private:
	FVector2D CameraInput;
	void MoveForward(float input);
	void MoveRight(float input);
	void PitchCamera(float AxisValue);
	void YawCamera(float AxisValue);
};
