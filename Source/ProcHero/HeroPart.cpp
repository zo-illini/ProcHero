// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroPart.h"

// Sets default values
AHeroPart::AHeroPart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHeroPart::BeginPlay()
{
	Super::BeginPlay();
	
	isValid = (Source != nullptr);

	if (isValid)
	{
		OriginalDistance = (Source->GetActorLocation() - GetActorLocation()).Size();
	}

	isMovingToTarget = true;


}

// Called every frame
void AHeroPart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (isMovingToTarget) 
	{
		MoveToTargetSimple(MoveToTargetLocation, MoveToTargetRotator, DeltaTime, 1);
	}
	else 
	{
		FollowSource();
	}
}

// Called to bind functionality to input
void AHeroPart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AHeroPart::FollowSource() 
{
	if (isValid)
	{
		FVector TargetedLocation = Source->GetActorRotation().Vector() * (-OriginalDistance) + Source->GetActorLocation();
		this->SetActorLocation((FMath::Lerp<FVector, float>(GetActorLocation(), TargetedLocation, DampingTranslationWeight)));

		FRotator TargetedRotation = UKismetMathLibrary::MakeRotFromX((Source->GetActorLocation() - GetActorLocation()).GetSafeNormal());

		// Rotation is not interpolated for now
		//FRotator MyRotation = GetActorRotation();
		//TargetedRotation = UKismetMathLibrary::RInterpTo(MyRotation, TargetedRotation, DeltaTime, 10);


		this->SetActorRotation(TargetedRotation);
	}
}

void AHeroPart::SetMoveToTarget(FVector VTarget, FRotator RTarget) 
{
	MoveToTargetLocation = VTarget;
	MoveToTargetRotator = RTarget;
	MoveToStartLocation = this->GetActorLocation();
	MoveToStartRotation = this->GetActorRotation();
	Timer = 0;
	isMovingToTarget = true;
}

void AHeroPart::MoveToTargetSimple(FVector VTarget, FRotator RTarget, float DeltaTime, float speed) 
{
	if (isMovingToTarget) 
	{
		FVector cur = UKismetMathLibrary::VInterpTo(MoveToStartLocation, VTarget, Timer, speed);
		this->SetActorLocation(cur);
		this->SetActorRotation(UKismetMathLibrary::RInterpTo(MoveToStartRotation, RTarget, Timer, speed));

		if ((cur - VTarget).Size() < 1)
		{
			this->SetActorLocation(VTarget);
			isMovingToTarget = false;
			OriginalDistance = (Source->GetActorLocation() - GetActorLocation()).Size();
		}

		Timer += DeltaTime;
	}
}
