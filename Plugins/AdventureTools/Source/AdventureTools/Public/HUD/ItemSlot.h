// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "Items/ItemManagerProvider.h"

#include "Blueprint/UserWidget.h"
#include "Components/Button.h"

#include "ItemSlot.generated.h"

class UInventoryItem;
class UImage;

DECLARE_DYNAMIC_DELEGATE(FItemSlotDelegate);

/**
 * 
 */
UCLASS()
class ADVENTURETOOLS_API UItemSlot : public UUserWidget, public IItemManagerProvider
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized() override;

public:
	FItemSlotDelegate OnItemClick;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "ItemSlot")
	UButton *ItemButton;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "ItemSlot")
	UImage *ItemSlot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemSlot")
	bool HasItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemSlot")
	UInventoryItem *InventoryItem;

	void AddItem(UInventoryItem* InventoryItem);

	void RemoveItem();

	UFUNCTION()
	void HandleOnClicked();

	UFUNCTION()
	void HandleOnHover();

	UFUNCTION()
	void HandleOnUnhover();
private:
	FSlateBrush SavedStyle;
	
	void SetButtonImageFromInventoryItem(const UInventoryItem* InventoryItem);
};
