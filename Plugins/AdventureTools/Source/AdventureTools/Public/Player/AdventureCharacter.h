// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "AdventurePlayerController.h"
#include "BarkText.h"
#include "PaperZDCharacter.h"
#include "PaperZDAnimInstance.h"
#include "AdventureCharacter.generated.h"

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
	
	//////////////////////////////////
	///
	/// MOVEMENT
	///

	/// Try to prevent the player character from falling through the floor due to
	/// misplaced geometry. Exposed mostly for debugging purposes.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess=true))
	float MinZValue = 5.0f;

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Gameplay)
	FVector2D LastNonZeroMovement = FVector2D::ZeroVector;
	
	FVector2D LastVelocity = FVector2D::ZeroVector;

	void TeleportToLocation(FVector NewLocation);

	void SetFacingDirection(EWalkDirection Direction);

	EWalkDirection GetFacingDirection();

	//////////////////////////////////
	///
	/// CAMERA
	///

	/// Setup the player follow camera
	/// @see CameraActorToSpawn
	UFUNCTION(BlueprintCallable)
	void SetupCamera();
};
