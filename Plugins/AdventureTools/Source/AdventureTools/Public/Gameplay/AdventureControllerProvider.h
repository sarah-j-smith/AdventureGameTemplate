// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AdventureControllerProvider.generated.h"

class AAdventureCharacter;
class AAdventurePlayerController;

/**
 * Fetch and cache the instance of the <code>AAdventurePlayerController</code>
 * and the <code>AAdventureCharacter</code>.
 */
UCLASS()
class ADVENTURETOOLS_API UAdventureControllerProvider : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "AdventureController", meta = (WorldContext = "WorldContextObject"))
	AAdventurePlayerController *GetAdventurePlayerController(UObject *WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "AdventureController", meta = (WorldContext = "WorldContextObject"))
	AAdventureCharacter *GetAdventureCharacter(UObject *WorldContextObject);
};
