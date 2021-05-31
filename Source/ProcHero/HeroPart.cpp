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
	
	isValid = (SourceCube != nullptr);

	if (isValid)
	{
		OriginalDistance = (SourceCube->GetActorLocation() - GetActorLocation()).Size();
	}

	isMovingToTarget = true;


}

// Called every frame
void AHeroPart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (isFollowing) 
	{
		FollowSource();
	}
	else
	{
		MoveToTarget(MoveToTargetLocation, MoveToTargetRotator, DeltaTime, 2);
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
		FVector TargetedLocation = SourceCube->GetActorRotation().Vector() * (-OriginalDistance) + SourceCube->GetActorLocation();
		this->SetActorLocation((FMath::Lerp<FVector, float>(GetActorLocation(), TargetedLocation, DampingTranslationWeight)));

		FRotator MyRotation = GetActorRotation();
		FRotator TargetedRotation = UKismetMathLibrary::MakeRotFromX((SourceCube->GetActorLocation() - GetActorLocation()).GetSafeNormal());

		// Rotation is not interpolated for now
		//TargetedRotation = UKismetMathLibrary::RInterpTo(MyRotation, TargetedRotation, DeltaTime, 10);


		this->SetActorRotation(TargetedRotation);
	}
}

void AHeroPart::MoveToTarget(FVector VTarget, FRotator RTarget, float DeltaTime, float speed) 
{
	if (isMovingToTarget) 
	{
		FVector cur = UKismetMathLibrary::VInterpTo(this->GetActorLocation(), VTarget, DeltaTime, speed);
		this->SetActorLocation(cur);
		this->SetActorRotation(UKismetMathLibrary::RInterpTo(this->GetActorRotation(), RTarget, DeltaTime, speed));

		if ((cur - VTarget).Size() < 1)
		{
			this->SetActorLocation(VTarget);
			isMovingToTarget = false;
		}
	}
}
