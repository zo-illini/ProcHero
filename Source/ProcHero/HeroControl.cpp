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

	TryEnableInput();

	if (Possessed && !CurrentVelocity.IsZero()) 
	{
		SetActorLocation(GetActorLocation() + CurrentVelocity * DeltaTime);
	}

	if (Possessed)
	{
		FRotator NewRotation = FRotator(0, DeltaAngle * DeltaTime, 0) + GetActorRotation();
		SetActorRotation(NewRotation);
	}
	
}

// Called to bind functionality to input
void AHeroControl::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("Move_Forward", this, &AHeroControl::MoveForward);
	//InputComponent->BindAxis("Move_Backward", this, &AHeroControl::MoveBackward);
	InputComponent->BindAxis("Move_Side", this, &AHeroControl::MoveSide);
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
			(Cast<AHeroPart>(AllHeroParts[i]))->SetMoveToTarget(TargetedLocations[i], GetActorRotation(), 2);
		}
	}
}

void AHeroControl::TryEnableInput()
{
	bool AllPartsInPlace = true;
	for (AActor* actor : AllHeroParts)
	{
		AllPartsInPlace &= Cast<AHeroPart>(actor)->isFollowing;
	}

	if (AllPartsInPlace)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Number of spline points not equal to 3"));

		UGameplayStatics::GetPlayerController(GetWorld(), 0)->Possess(this);
		Possessed = true;
	}
}

void AHeroControl::MoveForward(float AxisValue) 
{
	if (AxisValue >= 0) 
	{
		CurrentVelocity = GetActorRotation().Vector() * AxisValue * ForwardVelocity;
	}
	else 
	{
		CurrentVelocity = GetActorRotation().Vector() * AxisValue * BackwardVelocity;
	}
}

void AHeroControl::MoveSide(float AxisValue) 
{
	DeltaAngle = AxisValue * TurnVelocity;
}
