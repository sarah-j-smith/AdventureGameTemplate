// (c) 2025 Sarah Smith

#include "Player/FollowCamera.h"

#include "Kismet/KismetMathLibrary.h"
#include "Player/AdventureCharacter.h"

AFollowCamera::AFollowCamera()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FollowCameraBase = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerDetectorSphere"));
	SetRootComponent(FollowCameraBase);
	    
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
    
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);

	CameraConfines = CreateDefaultSubobject<UBoxComponent>(TEXT("CameraConfines"));
	CameraConfines->SetupAttachment(FollowCameraBase);

	FollowCameraBase->SetCollisionProfileName(FName("NoCollision"));
}

// Called when the game starts or when spawned
void AFollowCamera::BeginPlay()
{
	Super::BeginPlay();

	SetupCameraConfines();
}

// Called every frame
void AFollowCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FollowPlayer(DeltaTime);
}

void AFollowCamera::SetupCameraConfines()
{
	check(CameraConfines);
	checkf(ConfinesOfCamera != FVector::ZeroVector, TEXT("Camera confines have not been set!"));
	CameraConfines->SetBoxExtent(ConfinesOfCamera);
	InitialiseCameraConfines();
}

void AFollowCamera::FollowPlayer(float DeltaTime)
{
	if (!IsValid(PlayerCharacter)) return;

	FVector PlayerCharacterPosition = PlayerCharacter->GetActorLocation();
	FVector CamTargetPosition;
	CamTargetPosition.X = UKismetMathLibrary::FClamp(PlayerCharacterPosition.X, ConfineMin.X, ConfineMax.X);
	CamTargetPosition.Y = UKismetMathLibrary::FClamp(PlayerCharacterPosition.Y, ConfineMin.Y, ConfineMax.Y);
	FVector CameraCurrentPosition = GetActorLocation();
	FVector CameraNewPosition;
	CameraNewPosition.X = UKismetMathLibrary::FInterpTo(CameraCurrentPosition.X,
		CamTargetPosition.X, DeltaTime, FollowSpeed);
	CameraNewPosition.Y = UKismetMathLibrary::FInterpTo(CameraCurrentPosition.Y,
	CamTargetPosition.Y, DeltaTime, FollowSpeed);
	
	SetActorLocation(CameraNewPosition);
}

void AFollowCamera::SetupCamera()
{
	if (!IsValid(PlayerCharacter)) return;
	
	FVector CameraCurrentPosition = GetActorLocation();
	FVector PlayerCharacterPosition = PlayerCharacter->GetActorLocation();
	FVector CamTargetPosition;
	CamTargetPosition.X = UKismetMathLibrary::FClamp(PlayerCharacterPosition.X, ConfineMin.X, ConfineMax.X);
	CamTargetPosition.Y = UKismetMathLibrary::FClamp(PlayerCharacterPosition.Y, ConfineMin.Y, ConfineMax.Y);
	CamTargetPosition.Z = CameraCurrentPosition.Z;
	
	SetActorLocation(CamTargetPosition);
}

void AFollowCamera::InitialiseCameraConfines()
{
	if (!CameraConfines) return;
	if (!CameraComponent) return;
	if (bConfinesInitialised) return;

	const FVector ConfinesBoxMax = CameraConfines->Bounds.Origin + CameraConfines->Bounds.BoxExtent;
	const FVector ConfinesBoxMin = CameraConfines->Bounds.Origin - CameraConfines->Bounds.BoxExtent;

	float HalfCamWidth = CameraComponent->OrthoWidth * 0.5f;
	float HalfCamHeight = (CameraComponent->OrthoWidth / CameraComponent->AspectRatio) * 0.5;
	
	if (CameraOrientation == ECameraOrientation::XAxisIsOrthoWidth)
	{
		ConfineMax.X = ConfinesBoxMax.X - HalfCamWidth;
		ConfineMax.Y = ConfinesBoxMax.Y - HalfCamHeight;
		ConfineMin.X = ConfinesBoxMin.X + HalfCamWidth;
		ConfineMin.Y = ConfinesBoxMin.Y + HalfCamHeight;
	} else
	{
		ConfineMax.X = ConfinesBoxMax.X - HalfCamHeight;
		ConfineMax.Y = ConfinesBoxMax.Y - HalfCamWidth;
		ConfineMin.X = ConfinesBoxMin.X + HalfCamHeight;
		ConfineMin.Y = ConfinesBoxMin.Y + HalfCamWidth;
	}

	bConfinesInitialised = true;
}

