// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Enums/CameraOrientation.h"

#include "FollowCamera.generated.h"

class AAdventureCharacter;
enum class ECameraOrientation : uint8;

UCLASS()
class ADVENTURETOOLS_API AFollowCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	AFollowCamera();
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	//////////////////////////////////
	///
	/// FOLLOW CAMERA PROPERTIES
	///

	/**
	 * Camera base. This is moved to change the location of the camera.
	 * The camera should not be moved directly as that would disable any
	 * spring arm effects.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FollowCamera")
	USphereComponent *FollowCameraBase;

	/**
	 * Camera should be set to orthographic, with fixed aspect ratio
	 * of the game scene width divided by the game scene height, eg 2.21.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FollowCamera")
	UCameraComponent *CameraComponent;

	/**
	 * Camera Arm - use this to set the rotation and position of the camera,
	 * and effects like camera lag. Do not set rotation and position directly
	 * on the camera itself.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FollowCamera")
	USpringArmComponent *SpringArmComponent;

	/**
	 * Box to help visualize limits for the camera, these are the bounds that the camera 
	 * viewport will be contained within. If this volumes extents are set, ensure they
	 * match the `ConfinesOfCamera` below.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FollowCamera")
	UBoxComponent *CameraConfines;

	/**
	 * Values to control the Camera Confines. The `CameraConfines` box will be set to these
	 * values at run-time.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FollowCamera")
	FVector ConfinesOfCamera;

	/**
	 * Is the camera aligned width-wise with the Y-Axis (Unreal 5.6 and later) or the X-Axis?
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FollowCamera")
	ECameraOrientation CameraOrientation = ECameraOrientation::YAxisIsOrthoWidth;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FollowCamera")
	float FollowSpeed = 12.0f;

	//////////////////////////////////
	///
	/// INITIALIZATION
	///

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FollowCamera")
	TObjectPtr<AAdventureCharacter> PlayerCharacter;

	/// Define the CameraConfines box from the screen values entered into
	/// the blueprint for the `ConfinesOfCamera`.
	void SetupCameraConfines();

	/// Jump the camera straight to a position as close as possible to
	/// the player within the confines.
	void SetupCamera();
	
	//////////////////////////////////
	///
	/// CAMERA FOLLOW BEHAVIOUR
	///
	
	/// Lerp the camera to a position as close as possible to the player
	/// within the confines. Call from Tick()
	void FollowPlayer(float DeltaTime);

private:
	bool bConfinesInitialised = false;
	
	/// Initialise the control values for the box that defines the camera confines.
	void InitialiseCameraConfines();

	/// Confines of a box that the POSITION (center/pivot) of the camera must be in.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FollowCamera", Meta=(AllowPrivateAccess="true"))
	FVector ConfineMax;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="FollowCamera", Meta=(AllowPrivateAccess="true"))
	FVector ConfineMin;
};
