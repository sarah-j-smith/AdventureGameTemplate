// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AdventureGameModeBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FScoreDelegate, int32, Score);

/**
 * 
 */
UCLASS()
class ADVENTURETOOLS_API AAdventureGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AAdventureGameModeBase();
	
	virtual void BeginPlay() override;

	FScoreDelegate ScoreDelegate;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	int32 Score = 0;

	UFUNCTION(BlueprintCallable)
	void AddToScore(int32 Increment);
};
