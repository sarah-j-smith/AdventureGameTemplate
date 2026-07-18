// (c) 2026 Storybridge Games


#include "RoomSceneryFactory.h"

#include "HotSpots/RoomScenery.h"

#define LOCTEXT_NAMESPACE "FAdventureTypesModule"

URoomSceneryFactory::URoomSceneryFactory()
{
	DisplayName = LOCTEXT("AdventureTypes_RoomSceneryName", "Room Scenery");
	NewActorClass = ARoomScenery::StaticClass();
	SpawnPositionOffset = FVector(50, 0, 0);
	bUseSurfaceOrientation = true;
}

#undef LOCTEXT_NAMESPACE