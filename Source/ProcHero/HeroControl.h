// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HeroPart.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "HeroControl.generated.h"

UCLASS()
class PROCHERO_API AHeroControl : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHeroControl();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitializeTargetedLocations();

	void InitializeSources();

	void StartMovingAllParts();

	void TryEnableInput();

	void StartSnakeMove();

	void SnakeMove(float DeltaTime);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere)
		USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere)
		USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere)
		UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere)
		bool AllPartsValid;

	UPROPERTY(EditAnywhere)
		TArray<AActor*> AllHeroParts;

	UPROPERTY(EditAnywhere)
		TArray<FVector> TargetedLocations;

	void MoveForward(float AxisValue);
	void MoveBackward(float AxisValue);
	void MoveSide(float AxisValue);
	void Transform();

	UPROPERTY(EditAnywhere)
		float ForwardVelocity;
	UPROPERTY(EditAnywhere)
		float BackwardVelocity;
	UPROPERTY(EditAnywhere)
		float TurnVelocity;

	UPROPERTY(VisibleAnywhere)
		FVector CurrentVelocity;
	UPROPERTY(VisibleAnywhere)
		float DeltaAngle;

	UPROPERTY(VisibleAnywhere)
		bool Possessed;

	UPROPERTY(EditAnywhere)
		bool CanSnakeMove;

	UPROPERTY(EditAnywhere)
		bool IsSnakeMoving;

	UPROPERTY(VisibleAnywhere)
		float SnakeMoveTimer;
	UPROPERTY(EditAnywhere)
		float SnakeMoveTime;

	UPROPERTY(EditAnywhere)
		float SnakeMoveAngle;

	UPROPERTY(VisibleAnywhere)
		float SnakeMoveStartAngle;

	UPROPERTY(VisibleAnywhere)
		int SnakeMoveDirection;

	UPROPERTY(EditAnywhere)
		float SnakeMoveSpeed;

};
