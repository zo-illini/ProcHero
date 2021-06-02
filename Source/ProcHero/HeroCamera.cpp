// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroCamera.h"

// Sets default values
AHeroCamera::AHeroCamera()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHeroCamera::BeginPlay()
{
	Super::BeginPlay();
	
	FixDistance = FollowTarget->GetActorLocation() - GetActorLocation();
}

// Called every frame
void AHeroCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorLocation(FollowTarget->GetActorLocation() - FixDistance);

}

// Called to bind functionality to input
void AHeroCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

