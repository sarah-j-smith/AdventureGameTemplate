// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/AdventureControllerProvider.h"
#include "UObject/Object.h"
#include "MockAdventureControllerProvider.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTURETOOLS_API UMockAdventureControllerProvider : public UAdventureControllerProvider
{
	GENERATED_BODY()
public:
	UPROPERTY()
	AAdventurePlayerController *AdventurePlayerController;
	
	UPROPERTY()
	AAdventureCharacter *AdventureCharacter;

	virtual AAdventurePlayerController* GetAdventurePlayerController(UObject* WorldContextObject) override
	{
		return AdventurePlayerController;
	}
	
	virtual AAdventureCharacter* GetAdventureCharacter(UObject* WorldContextObject) override
	{
		return AdventureCharacter;
	}
};
