// (c) 2025 Sarah Smith

#include "Player/AdventureCharacter.h"

#include "AdventureTools.h"
#include "GameUtils.h"
#include "WalkDirection.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Enums/InteractionType.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/FollowCamera.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"

AAdventureCharacter::AAdventureCharacter()
{
	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("Construct: AAdventureCharacter - Player movement controlled by AI"));

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(RootComponent);
	Sphere->SetSphereRadius(4.0);
	Sphere->SetRelativeLocation(FVector(0, -20, 0));
	
	OnClimbOverrideEndDelegate.BindUObject(this, &AAdventureCharacter::OnInteractAnimOverrideEnd);
	OnInteractOverrideEndDelegate.BindUObject(this, &AAdventureCharacter::OnInteractAnimOverrideEnd);
}

void AAdventureCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	OnSitOverrideEndDelegate.BindUObject(this, &AAdventureCharacter::OnSitAnimOverrideEnd);
	OnClimbOverrideEndDelegate.BindUObject(this, &AAdventureCharacter::OnClimbAnimOverrideEnd);
	OnInteractOverrideEndDelegate.BindUObject(this, &AAdventureCharacter::OnInteractAnimOverrideEnd);
	OnTurnLeftOverrideEndDelegate.BindUObject(this, &AAdventureCharacter::OnTurnLeftAnimOverrideEnd);
	OnTurnRightOverrideEndDelegate.BindUObject(this, &AAdventureCharacter::OnTurnRightAnimOverrideEnd);

	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("AdventureCharacter on construct"));
}

void AAdventureCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("AAdventureCharacter::BeginPlay"));
}

void AAdventureCharacter::Tick(float DeltaTime)
{
	static float DelayTimer = 0.0f;
	
	Super::Tick(DeltaTime);

	DelayTimer -= DeltaTime;
	if (DelayTimer <= 0.0f)
	{
		DelayTimer = 1.0f;
		UE_LOG(LogAdventureGame, Log, TEXT("AAdventureCharacter - position %s"), *GetActorLocation().ToString())
	}
	
	// This is used just to feed in the PaperZD Set direction for the sprite facing
	const UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	FVector2D Velocity = FVector2D(MovementComponent->Velocity.X, MovementComponent->Velocity.Y);
	if (FGameUtils::HasChangedMuch(Velocity, LastVelocity))
	{
		LastVelocity = Velocity;
		if (Velocity.SquaredLength() > 0)
		{
			LastNonZeroMovement = Velocity;
		}
	}
}

void AAdventureCharacter::Climb(EInteractTimeDirection InteractTimeDirection)
{
	LastInteractTimeDirection = InteractTimeDirection;
	UPaperZDAnimInstance *Anim = GetAnimInstance();
	Anim->PlayAnimationOverride(LastNonZeroMovement.X > 0 ? ClimbRightAnimationSequence : ClimbLeftAnimationSequence,
		FName("DefaultSlot"),
		InteractTimeDirection == EInteractTimeDirection::Forward ? 1.0f : -1.0f,
		0.0f,
		OnClimbOverrideEndDelegate);
}

void AAdventureCharacter::Interact(EInteractTimeDirection InteractTimeDirection)
{
	LastInteractTimeDirection = InteractTimeDirection;
	UPaperZDAnimInstance *Anim = GetAnimInstance();
	Anim->PlayAnimationOverride(LastNonZeroMovement.X > 0 ? InteractRightAnimationSequence : InteractLeftAnimationSequence,
		FName("DefaultSlot"),
		InteractTimeDirection == EInteractTimeDirection::Forward ? 1.0f : -1.0f,
		0.0f,
		OnInteractOverrideEndDelegate);
}

void AAdventureCharacter::Sit(EInteractTimeDirection InteractTimeDirection)
{
	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("AAdventureCharacter::Sit - %s"),
		*(TimeDirectionGetDescriptiveString(InteractTimeDirection)));

	LastInteractTimeDirection = InteractTimeDirection;
	UPaperZDAnimInstance *Anim = GetAnimInstance();
	Anim->PlayAnimationOverride(
		LastNonZeroMovement.X > 0 ? SitRightAnimationSequence : SitLeftAnimationSequence,
		FName("DefaultSlot"),
		InteractTimeDirection == EInteractTimeDirection::Forward ? 1.0f : -1.0f,
		0.0f,
		OnSitOverrideEndDelegate);
}

void AAdventureCharacter::TurnLeft(EInteractTimeDirection InteractTimeDirection)
{
	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("AAdventureCharacter::TurnLeft - %s"),
		*(TimeDirectionGetDescriptiveString(InteractTimeDirection)));

	LastInteractTimeDirection = InteractTimeDirection;
	UPaperZDAnimInstance *Anim = GetAnimInstance();
	Anim->PlayAnimationOverride(
		TurnLeftAnimationSequence,
		FName("DefaultSlot"),
		InteractTimeDirection == EInteractTimeDirection::Forward ? 1.0f : -1.0f,
		0.0f,
		OnTurnLeftOverrideEndDelegate);
}

void AAdventureCharacter::TurnRight(EInteractTimeDirection InteractTimeDirection)
{
	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("AAdventureCharacter::TurnRight - %s"),
		*(TimeDirectionGetDescriptiveString(InteractTimeDirection)));

	LastInteractTimeDirection = InteractTimeDirection;
	UPaperZDAnimInstance *Anim = GetAnimInstance();
	Anim->PlayAnimationOverride(
		TurnRightAnimationSequence,
		FName("DefaultSlot"),
		InteractTimeDirection == EInteractTimeDirection::Forward ? 1.0f : -1.0f,
		0.0f,
		OnTurnRightOverrideEndDelegate);
}

void AAdventureCharacter::OnInteractAnimOverrideEnd(bool completed)
{
	AnimationCompleteDelegate.Broadcast(EInteractionType::Interact, completed);
}

void AAdventureCharacter::OnClimbAnimOverrideEnd(bool completed)
{
	AnimationCompleteDelegate.Broadcast(EInteractionType::Climb, completed);
}

void AAdventureCharacter::OnSitAnimOverrideEnd(bool completed)
{
	AnimationCompleteDelegate.Broadcast(EInteractionType::Sit, completed);
}

void AAdventureCharacter::OnTurnLeftAnimOverrideEnd(bool completed)
{
	if (completed)
	{
		if (LastInteractTimeDirection == EInteractTimeDirection::Forward)
		{
			SetFacingDirection(EWalkDirection::Left);
		}
		else
		{
			SetFacingDirection(EWalkDirection::Down);
		}
	}
	AnimationCompleteDelegate.Broadcast(EInteractionType::TurnLeft, completed);
}

void AAdventureCharacter::OnTurnRightAnimOverrideEnd(bool completed)
{
	if (completed)
	{
		if (LastInteractTimeDirection == EInteractTimeDirection::Forward)
		{
			SetFacingDirection(EWalkDirection::Right);
		}
		else
		{
			SetFacingDirection(EWalkDirection::Down);
		}
	}
	AnimationCompleteDelegate.Broadcast(EInteractionType::TurnRight, completed);
}

void AAdventureCharacter::TeleportToLocation(FVector NewLocation)
{
	UCapsuleComponent *Capsule = GetCapsuleComponent();
	FVector PrevLocation = Capsule->GetComponentLocation();

	// in debug stop here to find out why player is falling through the floor
	// probably a misplaced door or player start object.
	if (PrevLocation.Z < MinZValue)
	{
		// Falling happens with increasingly negative z values.
		UE_LOG(LogAdventureGame, Verbose, TEXT("AAdventureCharacter::TeleportToLocation - %s"), *NewLocation.ToString());
	}
	const FVector TeleportLocation = FVector(NewLocation.X, NewLocation.Y,FMath::Clamp(NewLocation.Z, MinZValue, MaxZValue));
	SetActorLocation(TeleportLocation);
	UE_LOG(LogAdventureGame, Warning, TEXT("AAdventureCharacter::TeleportToLocation - %s"), *TeleportLocation.ToString());
	UE_LOG(LogAdventureGame, Warning, TEXT("   (Possibly clamped from - %s)"), *NewLocation.ToString());	
}

void AAdventureCharacter::SetFacingDirection(EWalkDirection Direction)
{
	switch (Direction)
	{
	case EWalkDirection::Left:
		LastNonZeroMovement = FVector2D(-1.0f, 0.0f);
		break;
	case EWalkDirection::Right:
		LastNonZeroMovement = FVector2D(1.0f, 0.0f);
		break;
	case EWalkDirection::Up:
		LastNonZeroMovement = FVector2D(0.0f, -1.0f);
		break;
	case EWalkDirection::Down:
		LastNonZeroMovement = FVector2D(0.0f, 1.0f);
	}
}

EWalkDirection AAdventureCharacter::GetFacingDirection()
{
	if (LastNonZeroMovement.X > 0.0f)
	{
		return EWalkDirection::Right;
	}
	else if (LastNonZeroMovement.X < 0.0f)
	{
		return EWalkDirection::Left;
	}
	else if (LastNonZeroMovement.Y > 0.0f)
	{
		return EWalkDirection::Up;
	}
	return EWalkDirection::Down;
}

void AAdventureCharacter::SetupCamera()
{
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);

	AActor *CameraActor = UGameplayStatics::GetActorOfClass(GetWorld(), AFollowCamera::StaticClass());
	AFollowCamera *Camera = Cast<AFollowCamera>(CameraActor);
	if (IsValid(Camera))
	{
		Camera->PlayerCharacter = this;
		APlayerCameraManager *CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
		CameraManager->SetViewTarget(Camera);
		Camera->SetupCamera();
		UE_LOG(LogAdventureGame, Verbose, TEXT("Camera loaded - level size: %s"), *(Camera->CameraConfines->GetLocalBounds().ToString()));
	}
	else
	{
		UE_LOG(LogAdventureGame, Warning, TEXT("AAdventureCharacter::SetupCamera failed - misplaced or no camera set in this level?"));
	}
}
