// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HeroPart.h"
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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere)
		bool AllPartsValid;

	UPROPERTY(EditAnywhere)
		TArray<AActor*> AllHeroParts;

	UPROPERTY(EditAnywhere)
		TArray<FVector> TargetedLocations;


};
