// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CameraOrientation.generated.h"

/**
 * Is the FollowCamera oriented so that horizontal - left to right as viewed through the camera lens - is on the
 * Y-Axis, or is horizontal on the X-Axis?  After Unreal Engine 5.6 the Top view is set with `YAxisIsOrthoWidth`
 * which should be the default. But this could change in some games or circumstances. 
 */
UENUM(BlueprintType)
enum class ECameraOrientation : uint8
{
	YAxisIsOrthoWidth = 0 UMETA(DisplayName="Y-Axis is Camera Ortho Width"),
	XAxisIsOrthoWidth = 1 UMETA(DisplayName="X-Axis is Camera Ortho Width"),
};