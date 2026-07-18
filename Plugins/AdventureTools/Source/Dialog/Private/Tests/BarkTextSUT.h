// (c) 2026 Storybridge Games

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "BarkText.h"
#include "Components/VerticalBox.h"
#include "BarkTextSUT.generated.h"

#define TARGET_REACHED 1
#define TIMED_OUT 2

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
	UBarkText *SUT;
	
	UBarkText *CreateBarkText(UWorld* World);
};
