// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AdventureAIController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMoveCompletedDelegate, EPathFollowingResult::Type, Result);

/**
 * 
 */
UCLASS()
class ADVENTURETOOLS_API AAdventureAIController : public AAIController
{
	GENERATED_BODY()
public:
	virtual void OnPossess(APawn* InPawn) override;
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	FMoveCompletedDelegate MoveCompletedDelegate;
};
