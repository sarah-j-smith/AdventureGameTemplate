// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "AdventurePlayerController.h"
#include "BarkText.h"
#include "PaperZDCharacter.h"
#include "PaperZDAnimInstance.h"
#include "Enums/CameraOrientation.h"
#include "AdventureCharacter.generated.h"

enum class ECameraOrientation : uint8;
enum class EWalkDirection : uint8;
class UWidgetComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCharacterAnimComplete, EInteractionType, Interaction, bool, Complete);

/**
 * 
 */
UCLASS()
class ADVENTURETOOLS_API AAdventureCharacter : public APaperZDCharacter
{
	GENERATED_BODY()

public:
	AAdventureCharacter();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	//////////////////////////////////
	///
	/// CUSTOM ANIMATIONS
	///

private:
	FZDOnAnimationOverrideEndSignature OnClimbOverrideEndDelegate;

	FZDOnAnimationOverrideEndSignature OnInteractOverrideEndDelegate;

	FZDOnAnimationOverrideEndSignature OnSitOverrideEndDelegate;
	
	FZDOnAnimationOverrideEndSignature OnTurnLeftOverrideEndDelegate;
	
	FZDOnAnimationOverrideEndSignature OnTurnRightOverrideEndDelegate;
	
public:
	/// Marker object to anchor bark text
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Barking)
	USphereComponent* Sphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Barking)
	FColor BarkColor = FColor::White;
	
	/// Animation sequence for interacting with objects
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Animation)
	UPaperZDAnimSequence *InteractLeftAnimationSequence;
	
	/// Animation sequence for interacting with objects
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Animation)
	UPaperZDAnimSequence *InteractRightAnimationSequence;

	/// Animation sequence for climbing on objects
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Animation)
	UPaperZDAnimSequence *ClimbLeftAnimationSequence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Animation)
	UPaperZDAnimSequence *ClimbRightAnimationSequence;
	
	/// Animation sequence for climbing on objects
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Animation)
	UPaperZDAnimSequence *SitLeftAnimationSequence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Animation)
	UPaperZDAnimSequence *SitRightAnimationSequence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Animation)
	UPaperZDAnimSequence *TurnRightAnimationSequence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Animation)
	UPaperZDAnimSequence *TurnLeftAnimationSequence;

	void Climb(EInteractTimeDirection InteractTimeDirection = EInteractTimeDirection::Forward);

	void Interact(EInteractTimeDirection InteractTimeDirection = EInteractTimeDirection::Forward);

	void Sit(EInteractTimeDirection InteractTimeDirection = EInteractTimeDirection::Forward);

	void TurnLeft(EInteractTimeDirection InteractTimeDirection = EInteractTimeDirection::Forward);
	
	void TurnRight(EInteractTimeDirection InteractTimeDirection = EInteractTimeDirection::Forward);

	FCharacterAnimComplete AnimationCompleteDelegate;
	
private:
	void OnInteractAnimOverrideEnd(bool completed);
	
	void OnClimbAnimOverrideEnd(bool completed);

	void OnSitAnimOverrideEnd(bool completed);

	void OnTurnLeftAnimOverrideEnd(bool completed);
	
	void OnTurnRightAnimOverrideEnd(bool completed);

	EInteractTimeDirection LastInteractTimeDirection;
	
	
public:
	//////////////////////////////////
	///
	/// MOVEMENT
	///

	/// Try to prevent the player character from falling through the floor due to
	/// misplaced geometry. Exposed mostly for debugging purposes.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement)
	float MinZValue = 0.01f;

	/// Try to prevent the player character from somehow being above the nav mesh.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement)
	float MaxZValue = 1.0f;
	
	/// Last movement vector for character to animate along. This value tracks 
	/// non-zero values of the MovementComponent's Velocity, which increases
	/// and decreases under control of the AI navigation system.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Gameplay)
	FVector2D LastNonZeroMovement = FVector2D::ZeroVector;
	
	/// Any movement component velocity below this amount is treated as being stationary
	/// and is ignored for the purposes of setting the facing direction. This should be
	/// about 10% of the player's walking speed.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement)
	double MinSpeed = 5.0;
	
	void SetLastNonZeroMovement(FVector2D NewLastNonZeroMovement);
	
	/// Direction vector in 2D for Sprite Facing. If the camera width dimension is aligned with
	/// the X-axis as in < Unreal 5.6, then this will be the same as `LastNonZeroMovement`. If
	/// the camera width is aligned with the Y-axis >= Unreal 5.6, then this is a rotated version
	/// of LastNonZeroMovement
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Gameplay)
	FVector2D AnimationDirection = FVector2D::ZeroVector;
	
	void SetAnimationDirection(FVector2D MovementValue);
	
	FVector2D LastVelocity = FVector2D::ZeroVector;

	void TeleportToLocation(FVector NewLocation);

	void SetFacingDirection(EWalkDirection Direction);

	EWalkDirection GetFacingDirection();
	
	/// This should match the FollowCamera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement)
	ECameraOrientation CameraOrientation = ECameraOrientation::YAxisIsOrthoWidth;

	//////////////////////////////////
	///
	/// CAMERA
	///

	/// Setup the player follow camera
	/// @see CameraActorToSpawn
	UFUNCTION(BlueprintCallable)
	void SetupCamera();
};
