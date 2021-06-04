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

	void TryEnablePhysics();


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
		APlayerController* PlayerController;

	UPROPERTY(VisibleAnywhere)
		bool AllPartsValid;

	UPROPERTY(EditAnywhere)
		TArray<AActor*> AllHeroParts;

	UPROPERTY(EditAnywhere)
		TArray<FVector> TargetedLocations;

	UPROPERTY(EditAnywhere)
		float ForwardForce;


	void MoveForward(float AxisValue);
	void MoveBackward(float AxisValue);
	void MoveSide(float AxisValue);
	void MouseTurn(float AxisValue);
	void MouseTurnUp(float AxisValue);
	void Transform();


};
