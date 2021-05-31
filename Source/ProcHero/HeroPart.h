// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "HeroPart.generated.h"

UCLASS()
class PROCHERO_API AHeroPart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHeroPart();

	void SetMoveToTarget(FVector VTarget, FRotator RTarget);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void FollowSource();

	void MoveToTargetSimple(FVector VTarget, FRotator RTarget, float DeltaTime, float speed);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere)
		bool isValid;

	UPROPERTY(VisibleAnywhere)
		bool isFollowing;

	UPROPERTY(EditAnywhere)
		AActor* Source;

	UPROPERTY(VisibleAnywhere)
		float OriginalDistance;

	UPROPERTY(EditAnywhere)
		float DampingTranslationWeight;

	UPROPERTY(EditAnywhere)
		bool isMovingToTarget;

	UPROPERTY(VisibleAnywhere)
		FVector MoveToStartLocation;

	UPROPERTY(VisibleAnywhere)
		FRotator MoveToStartRotation;

	UPROPERTY(EditAnywhere)
		FVector MoveToTargetLocation;

	UPROPERTY(EditAnywhere)
		FRotator MoveToTargetRotator;

	UPROPERTY(VisibleAnywhere)
		float Timer;

};
