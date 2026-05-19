// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "AdventureGameModeBase.h"
#include "AdventureGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTURETOOLS_API AAdventureGameMode : public AAdventureGameModeBase
{
    GENERATED_BODY()
protected:
    virtual void BeginPlay() override;
	
public:
    virtual void OnConstruction(const FTransform& Transform) override;

    /// A switch to teleport to new locations in the game instead using AI to
    /// walk there over the nav graph. Can be used with non-AI / non-nav mesh
    /// movement like an empty or invisible character. Also useful for testing.
    /// Use this setting in preference to the one on the command manager.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool bTeleportInsteadOfWalk = false;
};
