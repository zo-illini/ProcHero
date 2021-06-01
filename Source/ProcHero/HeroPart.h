// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SplineComponent.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "HeroPart.generated.h"

UCLASS()
class PROCHERO_API AHeroPart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHeroPart();

	void SetMoveToTarget(FVector VTarget, FRotator RTarget, int MoveMode);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void FollowSource();

	void MoveToTarget(float DeltaTime);

	void MoveToTargetSimple(FVector VTarget, FRotator RTarget, float DeltaTime, float speed);

	void MoveToTargetSpline(float DeltaTime, float speed);

	void SampleSpline(USplineComponent * Spline, float DeltaTime, float speed);

	void RandomizeSpline(USplineComponent* Spline, float RangeRatio);

	void MoveToTargetGravity(float DeltaTime);

	int MaxSplineSampleNum;

	int SplineSampleNum;

	int SplineSamplePtr;

	TArray<FVector> SplineSamples;

	bool HasSampledSpline;


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

	UPROPERTY(EditAnywhere)
		int MoveMode;

	UPROPERTY(VisibleAnywhere)
		float MoveToTimer;

	UPROPERTY(VisibleAnywhere)
		USplineComponent* SplineComponent;

	UPROPERTY(EditAnywhere)
		float SplineMoveSpeed;

	UPROPERTY(VisibleAnywhere)
		int SplineMoveCounter;

	UPROPERTY(EditAnywhere)
		float GravityScale;

	UPROPERTY(VisibleAnywhere)
		FVector Velocity;

	UPROPERTY(EditAnywhere)
		float ConstantGravityBoundary;

	UPROPERTY(EditAnywhere)
		float ConstantGravityScale;
};
