// (c) 2025 Sarah Smith


#include "Player/AdventureAIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "AdventureTools.h"
#include "AITypes.h"

void AAdventureAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("AAdventureAIController::OnPossess"));
}

void AAdventureAIController::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("AAdventureAIController::OnConstruction"));
}

void AAdventureAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	UE_LOG(LogAdventureGame, Display, TEXT("OnMoveCompleted %s - %s"), *RequestID.ToString(), *Result.ToString());
	MoveCompletedDelegate.Broadcast(Result.Code);
}
