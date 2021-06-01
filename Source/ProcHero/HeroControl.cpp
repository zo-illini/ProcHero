// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroControl.h"

// Sets default values
AHeroControl::AHeroControl()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHeroControl::BeginPlay()
{
	Super::BeginPlay();

	AllPartsValid = true;

	for (AActor* actor : AllHeroParts) 
	{
		AllPartsValid &= (actor != nullptr);
	}

	InitializeTargetedLocations();
	InitializeSources();

	StartMovingAllParts();
}

// Called every frame
void AHeroControl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AHeroControl::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AHeroControl::InitializeTargetedLocations() 
{
	// 简单的初始化排成一条长蛇的TargetedLocations
	FVector Offset = GetActorRotation().Vector() * -120;
	FVector Target = this->GetActorLocation() + Offset/1.5;
	for (int i = 0; i < AllHeroParts.Num(); i++) 
	{
		TargetedLocations.Add(Target);
		Target += Offset;
	}
}

void AHeroControl::InitializeSources() 
{
	check(AllHeroParts.Num() > 0);
	(Cast<AHeroPart>(AllHeroParts[0]))->Source = this;
	for (int i = 1; i < AllHeroParts.Num(); i++) 
	{
		(Cast<AHeroPart>(AllHeroParts[i]))->Source = AllHeroParts[i - 1];
	}
}

void AHeroControl::StartMovingAllParts() 
{
	if (AllPartsValid)
	{
		check(AllHeroParts.Num() == TargetedLocations.Num());
		for (int i = 0; i < AllHeroParts.Num(); i++)
		{
			(Cast<AHeroPart>(AllHeroParts[i]))->SetMoveToTarget(TargetedLocations[i], GetActorRotation(), true);
			(Cast<AHeroPart>(AllHeroParts[i]))->MoveMode = 2;
		}
	}
}

