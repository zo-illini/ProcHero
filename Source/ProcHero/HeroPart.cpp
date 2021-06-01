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
	
	//isMovingToTarget = false;
	isFollowing = false;

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
		if (!HasSampledSpline)
		{
			SampleSpline(SplineComponent, DeltaTime, SplineMoveSpeed);
		}
		else 
		{
			MoveToTargetSpline(DeltaTime, SplineMoveSpeed);
		}

	}
	else 
	{
		if (isFollowing)
		{
			FollowSource();
		}
		else 
		{
			// Make sure source is in position before switching to the following mode
			AHeroPart* ptr = Cast<AHeroPart>(Source);
			if (ptr && !ptr->isMovingToTarget) 
			{
				OriginalDistance = (Source->GetActorLocation() - GetActorLocation()).Size();
				isFollowing = true;
			}
			else if (ptr == nullptr) //First hero part, source is hero control
			{
				OriginalDistance = (Source->GetActorLocation() - GetActorLocation()).Size();
				isFollowing = true;
			}
		}
	}
}

// Called to bind functionality to input
void AHeroPart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AHeroPart::FollowSource() 
{
	check(Source);

	FVector TargetedLocation = Source->GetActorRotation().Vector() * (-OriginalDistance) + Source->GetActorLocation();
	this->SetActorLocation((FMath::Lerp<FVector, float>(GetActorLocation(), TargetedLocation, DampingTranslationWeight)));

	FRotator TargetedRotation = UKismetMathLibrary::MakeRotFromX((Source->GetActorLocation() - GetActorLocation()).GetSafeNormal());

	// Rotation is not interpolated for now
	//FRotator MyRotation = GetActorRotation();
	//TargetedRotation = UKismetMathLibrary::RInterpTo(MyRotation, TargetedRotation, DeltaTime, 10);


	this->SetActorRotation(TargetedRotation);
}

void AHeroPart::SetMoveToTarget(FVector VTarget, FRotator RTarget, bool UsingSplineMove) 
{
	MoveToTargetLocation = VTarget;
	MoveToTargetRotator = RTarget;
	MoveToStartLocation = this->GetActorLocation();
	MoveToStartRotation = this->GetActorRotation();
	MoveToTimer = 0;

	if (UsingSplineMove) 
	{
		SplineComponent->SetLocationAtSplinePoint(2, MoveToTargetLocation, ESplineCoordinateSpace::World, true);
		RandomizeSpline(SplineComponent, 0.5f);
		SplineSamplePtr = 0;
	}

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
		}

		MoveToTimer += DeltaTime;
	}
}

void AHeroPart::MoveToTargetSpline(float DeltaTime, float speed) 
{
	if (SplineSamplePtr < SplineSampleNum) 
	{
		SetActorLocation(SplineSamples[SplineSamplePtr]);
		// This makes sure rotation is done when the object reaches the last spline sample point
		this->SetActorRotation(UKismetMathLibrary::RInterpTo(MoveToStartRotation, MoveToTargetRotator, (float)SplineSamplePtr / SplineSampleNum, 1));
		MoveToTimer += DeltaTime;
		SplineSamplePtr++;
	}
	else if (SplineSamplePtr == SplineSampleNum)
	{
		isMovingToTarget = false;
		SplineSamplePtr++;
	}
}

/*
	A utility function that creates the sampled points on spline SplineComponent.
	Each sampled points is seperated by step = DeltaTime x speed
	When sampling is done, HasSampled is set to true and the results are stored in SplineSamples
*/

void AHeroPart::SampleSpline(USplineComponent * Spline, float DeltaTime, float speed) 
{
	check(Spline);

	// adding Delta to avoid divided-by-zero
	float step = DeltaTime * speed + DELTA;

	SplineSampleNum = (int)(Spline->GetSplineLength() / step);

	if (SplineSampleNum > MaxSplineSampleNum) 
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Spline sampling speed is too small. Endpoint might not be reached"));
		SplineSampleNum = MaxSplineSampleNum;
	}

	for (int i = 0; i < SplineSampleNum; i++)
	{
		SplineSamples[i] = Spline->GetLocationAtDistanceAlongSpline(i * step, ESplineCoordinateSpace::World);
	}
		
	HasSampledSpline = true;
}

void AHeroPart::RandomizeSpline(USplineComponent* Spline, float RangeRatio) 
{
	if (Spline->GetNumberOfSplinePoints() != 3) 
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Number of spline points not equal to 3"));
		return;
	}
	
	ESplineCoordinateSpace::Type space = ESplineCoordinateSpace::World;


	FVector Center = (Spline->GetLocationAtSplinePoint(0, space) + Spline->GetLocationAtSplinePoint(2, space)) / 2;
	float MaxRadius = RangeRatio * (Spline->GetLocationAtSplinePoint(3, space) - Spline->GetLocationAtSplinePoint(0, space)).Size();
	FVector Target = Center + FMath::VRand() * (FMath::RandRange(0, 1) * MaxRadius);

	Spline->SetLocationAtSplinePoint(1, Target, space, true);

}
