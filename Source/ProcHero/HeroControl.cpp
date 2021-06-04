// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroControl.h"

// Sets default values
AHeroControl::AHeroControl()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!SphereComponent)
	{
		SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereRoot"));
		RootComponent = SphereComponent;
	}
	if (!MeshComponent) 
	{
		MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
		if (Mesh.Succeeded()) 
		{
			MeshComponent->SetStaticMesh(Mesh.Object);
		}
		MeshComponent->SetupAttachment(RootComponent);
	}
	if (!SpringArmComponent) 
	{
		SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
		SpringArmComponent->SetupAttachment(RootComponent);
	}
	if (!CameraComponent)
	{
		CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
		CameraComponent->SetupAttachment(SpringArmComponent);
	}
	SpringArmComponent->TargetArmLength = 300;
	SpringArmComponent->SetRelativeRotation(FRotator(-20, 0, 0));
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->bUsePawnControlRotation = true;

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

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->Possess(this);

}

// Called every frame
void AHeroControl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TryEnablePhysics();

	FindWall();
	
}

// Called to bind functionality to input
void AHeroControl::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("Move_Forward", this, &AHeroControl::MoveForward);
	InputComponent->BindAxis("Turn", this, &AHeroControl::MouseTurn);
	InputComponent->BindAxis("TurnUp", this, &AHeroControl::MouseTurnUp);
	InputComponent->BindAction("Transform",  IE_Pressed, this, &AHeroControl::Transform);
}

void AHeroControl::InitializeTargetedLocations() 
{
	// 简单的初始化排成一条长蛇的TargetedLocations
	//FVector Offset = GetActorRotation().Vector() * -60;
	float radius = Cast<AHeroPart>(AllHeroParts[0])->SphereComponent->GetUnscaledSphereRadius();
	FVector Offset = GetActorRotation().Vector() * -1 * (radius+1);
	FVector Target = this->GetActorLocation() + Offset;
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

void AHeroControl::TryEnablePhysics()
{
	bool AllPartsInPlace = true;
	for (AActor* actor : AllHeroParts)
	{
		AllPartsInPlace &= (Cast<AHeroPart>(actor)->MyStatus == Status::Following);
	}

	if (AllPartsInPlace)
	{
		SphereComponent->SetSimulatePhysics(true);
		SphereComponent->SetEnableGravity(true);
		SphereComponent->SetCollisionProfileName("BlockAll");
	}
}

void AHeroControl::FindWall() 
{
	FHitResult Out;
	FVector CameraDirection = CameraComponent->GetForwardVector().GetSafeNormal();
	FVector Head = GetActorLocation();
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	FCollisionResponseParams ResponseParams;
	if (GetWorld()->LineTraceSingleByChannel(Out, Head, Head + CameraDirection * (SphereComponent->GetUnscaledSphereRadius() + 10), ECollisionChannel::ECC_WorldStatic, Params, ResponseParams)) 
	{
		IsOnWall = true;
		ForwardDirection = FVector(0, 0, 1);
		//ForwardDirection = FVector::VectorPlaneProject(CameraDirection, Out.Normal);
		//ForwardDirection.Z = FMath::Abs(ForwardDirection.Z);
	}
	else 
	{
		//ForwardDirection = FVector::VectorPlaneProject(CameraDirection, FVector(0, 0, 1));
		ForwardDirection = CameraDirection;
	}
	ForwardDirection.Z = FMath::Clamp<float>(ForwardDirection.Z, 0, ForwardDirection.Z);


	
}

void AHeroControl::MoveForward(float AxisValue) 
{
	if (SphereComponent->IsSimulatingPhysics()) 
	{
		if (AxisValue > 0) 
		{
			SphereComponent->AddForce(ForwardDirection.GetSafeNormal() * ForwardForce, NAME_None, true);
			//SphereComponent->AddForce(FVector::VectorPlaneProject(CameraComponent->GetForwardVector(), FVector(0, 0, 1)).GetSafeNormal() * ForwardForce, NAME_None, true);
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Force Applied"));
		}
	}
}

void AHeroControl::MouseTurn(float AxisValue) 
{
	AddControllerYawInput(AxisValue);
}

void AHeroControl::MouseTurnUp(float AxisValue)
{
	AddControllerPitchInput(AxisValue);
}


void AHeroControl::Transform() 
{
	InitializeTargetedLocations();
	InitializeSources();
	StartMovingAllParts();
}
