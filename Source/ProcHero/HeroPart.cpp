// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroPart.h"

// Sets default values
AHeroPart::AHeroPart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!SplineComponent)
	{
		SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
		SplineComponent->SetupAttachment(RootComponent);
	}
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
	isFollowing = true;
	MaxSplineSampleNum = 1000;
	for (int i = 0; i < MaxSplineSampleNum; i++)
	{
		SplineSamples.Add(FVector(0, 0, 0));
	}
}

// Called every frame
void AHeroPart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (isMovingToTarget) 
	{
		//MoveToTargetSimple(MoveToTargetLocation, MoveToTargetRotator, DeltaTime, 1);
		MoveToTargetSpline(DeltaTime, SplineMoveSpeed);

	}
	else if (isFollowing)
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
	MoveToTimer = 0;
	isMovingToTarget = true;
}

void AHeroPart::MoveToTargetSimple(FVector VTarget, FRotator RTarget, float DeltaTime, float speed) 
{
	if (isMovingToTarget) 
	{
		FVector cur = UKismetMathLibrary::VInterpTo(MoveToStartLocation, VTarget, MoveToTimer, speed);
		this->SetActorLocation(cur);
		this->SetActorRotation(UKismetMathLibrary::RInterpTo(MoveToStartRotation, RTarget, MoveToTimer, speed));

		if ((cur - VTarget).Size() < 1)
		{
			this->SetActorLocation(VTarget);
			isMovingToTarget = false;
			OriginalDistance = (Source->GetActorLocation() - GetActorLocation()).Size();
		}

		MoveToTimer += DeltaTime;
	}
}

void AHeroPart::MoveToTargetSpline(float DeltaTime, float speed) 
{
	if (!HasSampledSpline) 
	{
		SampleSpline(DeltaTime, speed);
	}
	
	if (MoveToTimer < SplineSampleNum) 
	{
		SetActorLocation(SplineSamples[MoveToTimer]);
		MoveToTimer++;
	}
	else if (MoveToTimer == SplineSampleNum) 
	{
		isMovingToTarget = false;
		MoveToTimer++;
		OriginalDistance = (Source->GetActorLocation() - GetActorLocation()).Size();
	}
}

/*
	A utility function that creates the sampled points on spline SplineComponent.
	Each sampled points is seperated by step = DeltaTime x speed
	When sampling is done, HasSampled is set to true and the results are stored in SplineSamples
*/

void AHeroPart::SampleSpline(float DeltaTime, float speed) 
{
	check(SplineComponent);

	// adding Delta to avoid divided-by-zero
	float step = DeltaTime * speed + DELTA;

	SplineSampleNum = (int)(SplineComponent->GetSplineLength() / step);

	if (SplineSampleNum > MaxSplineSampleNum) 
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Spline sampling speed is too small. Endpoint might not be reached"));
		SplineSampleNum = MaxSplineSampleNum;
	}

	for (int i = 0; i < SplineSampleNum; i++)
	{
		SplineSamples[i] = SplineComponent->GetLocationAtDistanceAlongSpline(i * step, ESplineCoordinateSpace::World);
	}
		
	HasSampledSpline = true;
}
