// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ButtonFixture.generated.h"

class UItemSlot;
class UItem;

/**
 * 
 */
UCLASS()
class ADVENTURETOOLS_API UButtonFixture : public UUserWidget
{
	GENERATED_UCLASS_BODY()
	
public:
	TSubclassOf<UUserWidget> ButtonClass;
	
	UPROPERTY()
	UItemSlot *Button;
	
	UPROPERTY()
	UItem *Item;
	
	UItemSlot *CreateItemSlotButton(UWorld *World);
};
