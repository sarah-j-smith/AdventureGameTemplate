// (c) 2026 Storybridge Games

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "BarkRequestFinishReason.h"
#include "BarkText.h"
#include "Components/VerticalBox.h"
#include "BarkTextSUT.generated.h"

#define TARGET_REACHED 1
#define TIMED_OUT 2

#define NO_VALID_RESULT -127;

#define NOT_COMPLETED -99
#define WAS_INTERRUPTED -86

/**
 * 
 */
UCLASS()
class DIALOG_API UBarkTextSUT : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UBarkTextSUT(const FObjectInitializer &ObjectInitializer);
	
	int TickUntil(float DeltaTime, float maxTime, std::function<bool(UBarkText*)> StopCondition);
	
	TSubclassOf<UUserWidget> BarkTextClass;
	
	UPROPERTY()
	UStringTable *TestTextTable;
	
	UPROPERTY()
	UBarkText *SUT;
	
	FBarkRequest *SetupTestData(const TArray<FText>& BarkTextArray);
	
	UBarkText *CreateBarkText(UWorld* World);
	
	int TickUntilChildCount(int Count, float MaxTime);
	
	int TickUntilTextExists(const FText& SearchText, float MaxTime);

	int CompletedUID = NO_VALID_RESULT;
	
	EBarkRequestFinishedReason FinishedReason = EBarkRequestFinishedReason::BarkAndEnd;
};
