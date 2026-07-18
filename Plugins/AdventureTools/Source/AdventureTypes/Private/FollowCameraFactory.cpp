// (c) 2026 Storybridge Games


#include "FollowCameraFactory.h"

#include "Player/FollowCamera.h"

#define LOCTEXT_NAMESPACE "FAdventureTypesModule"

UFollowCameraFactory::UFollowCameraFactory()
{
	DisplayName = LOCTEXT("AdventureTypes_FollowCameraActorName", "Follow Camera");
	NewActorClass = AFollowCamera::StaticClass();
	SpawnPositionOffset = FVector(50, 0, 0);
	bUseSurfaceOrientation = true;
}

#undef LOCTEXT_NAMESPACE