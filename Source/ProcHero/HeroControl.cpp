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
	SpringArmComponent->TargetArmLength = 500;
	SpringArmComponent->bInheritYaw = false;
	SpringArmComponent->SetRelativeRotation(FRotator(-20, 0, 0));
	SpringArmComponent->bDoCollisionTest = false;

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
	Possessed = true;

	SnakeMoveDirection = 1;
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
	if (Possessed && !IsSnakeMoving) 
	{
		float TargetYaw = SpringArmComponent->GetRelativeRotation().Yaw;
		float DeltaYaw = TargetYaw - GetActorRotation().Yaw;

		if (FMath::Abs(DeltaYaw) < 0.001) 
		{
			// 后退时的snakemove要从反向进入
			StartSnakeMove(CurrentVelocity.X *  DeltaYaw);

		}
		SetActorRotation(FRotator(0, TargetYaw, 0));
	}

	// Stop snake movement when mouse move(turning) or when not going forward
	if (FMath::Abs(DeltaAngle) > 0.01 || CurrentVelocity.IsZero()) 
	{
		IsSnakeMoving = false;
	}

	if (CanSnakeMove && IsSnakeMoving) 
	{
		SnakeMove(DeltaTime);
	}

	MoveCameraWithMouse(DeltaTime);
	
}

// Called to bind functionality to input
void AHeroControl::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("Move_Forward", this, &AHeroControl::MoveForward);
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
			(Cast<AHeroPart>(AllHeroParts[i]))->SetMoveToTarget(TargetedLocations[i], GetActorRotation(), 1);
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

	if (AxisValue == 0) 
	{
		IsSnakeMoving = false;
	}
}

void AHeroControl::Transform() 
{
	InitializeTargetedLocations();
	InitializeSources();
	StartMovingAllParts();
}

void AHeroControl::StartSnakeMove(float direction) 
{
	IsSnakeMoving = true;
	SnakeMoveStartAngle = GetActorRotation().Yaw;
	SnakeMoveTimer = 0;
	SnakeMoveDirection = FMath::Sign(direction);
	// Also start snake-moving left when this function is called with direction 0
	if (SnakeMoveDirection == 0)
		SnakeMoveDirection = 1;
}

void AHeroControl::SnakeMove(float DeltaTime) 
{
	FRotator CurRot = GetActorRotation();
	if (SnakeMoveTimer > SnakeMoveTime)
	{
		SnakeMoveDirection = -1;
	}
	else if (SnakeMoveTimer < -SnakeMoveTime)
	{
		SnakeMoveDirection = 1;
	}
	SnakeMoveTimer += SnakeMoveDirection * DeltaTime;
	SetActorRotation(CurRot + FRotator(0, SnakeMoveDirection * SnakeMoveSpeed * DeltaTime, 0));
}

void AHeroControl::MoveCameraWithMouse(float DeltaTime) 
{
	float MouseX, MouseY;
	PlayerController->GetInputMouseDelta(MouseX, MouseY);
	FRotator CameraRot = SpringArmComponent->GetRelativeRotation() + FRotator(0, MouseX * DeltaTime * CameraRotSpeed, 0);
	SpringArmComponent->SetRelativeRotation(CameraRot);
	DeltaAngle = MouseX * DeltaTime;
}