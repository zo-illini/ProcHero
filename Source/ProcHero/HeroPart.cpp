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

	// Calculated the constant gravity scale that achieves equilibrium on the boundary
	//ConstantGravityScale = GravityScale / (ConstantGravityBoundary * ConstantGravityBoundary);
}

// Called every frame
void AHeroPart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (isMovingToTarget) 
	{
		MoveToTarget(DeltaTime);

		//MoveToTargetSimple(MoveToTargetLocation, MoveToTargetRotator, DeltaTime, 1);

		/*if (!HasSampledSpline)
		{
			SampleSpline(SplineComponent, DeltaTime, SplineMoveSpeed);
		}
		else 
		{
			MoveToTargetSpline(DeltaTime, SplineMoveSpeed);
		}*/

		//MoveToTargetGravity(DeltaTime);

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

void AHeroPart::SetMoveToTarget(FVector VTarget, FRotator RTarget, int Mode) 
{
	MoveToTargetLocation = VTarget;
	MoveToTargetRotator = RTarget;
	MoveToStartLocation = this->GetActorLocation();
	MoveToStartRotation = this->GetActorRotation();
	MoveToTimer = 0;
	MoveMode = Mode;

	if (Mode == 1) 
	{
		SplineComponent->SetLocationAtSplinePoint(2, MoveToTargetLocation, ESplineCoordinateSpace::World, true);
		RandomizeSpline(SplineComponent, 0.5f);
		SplineSamplePtr = 0;
	}

	isMovingToTarget = true;
}

void AHeroPart::MoveToTarget(float DeltaTime) 
{
	switch (MoveMode) 
	{
	case 0:
		MoveToTargetSimple(MoveToTargetLocation, MoveToTargetRotator, DeltaTime, 1);
		break;
	case 1:
		if (!HasSampledSpline)
		{
			SampleSpline(SplineComponent, DeltaTime, SplineMoveSpeed);
		}
		else
		{
			MoveToTargetSpline(DeltaTime, SplineMoveSpeed);
		}
		break;
	case 2:
		MoveToTargetGravity(DeltaTime);
		break;
	}
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

/*
	Simulate gravational force for object
	Outside ConstantGravityBoundary, force is constant
	Inside the boundary, force is scaled with square of distance toward target.
	On the boundary, the two type of force are equal.
*/

void AHeroPart::MoveToTargetGravity(float DeltaTime) 
{
	FVector VToTarget = MoveToTargetLocation - GetActorLocation();
	float Distance = (VToTarget).Size();

	if (Distance < DELTA * 100) 
	{
		SetActorLocation(MoveToTargetLocation);
		isMovingToTarget = false;
		return;
	}

	FVector Gravity;
	if (Distance < ConstantGravityBoundary)
	{
		Gravity = GravityScale / (Distance * Distance) * VToTarget.GetSafeNormal();
	}
	else 
	{
		Gravity = ConstantGravityScale * VToTarget.GetSafeNormal();
	}

	// Update Velocity
	Velocity += Gravity * DeltaTime;
	// Update Position and Clamp to avoid overshotting
	FVector Delta = Velocity * DeltaTime;
	Delta = Delta.GetSafeNormal() * FMath::Clamp<float>(Delta.Size(), 0, VToTarget.Size());
	SetActorLocation(GetActorLocation() + Delta);

	// Simply lerp toward target rotation by distance
	FVector TotalDistance = MoveToTargetLocation - MoveToStartLocation;
	SetActorRotation(FMath::RInterpTo(MoveToStartRotation, MoveToTargetRotator, 1 - Distance / TotalDistance.Size(), 1));

}