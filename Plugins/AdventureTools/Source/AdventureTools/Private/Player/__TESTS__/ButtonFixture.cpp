// (c) 2026 Storybridge Games


#include "ButtonFixture.h"

#include "HUD/ItemSlot.h"
#include "Item.h"

UButtonFixture::UButtonFixture(const FObjectInitializer &ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UItemSlot> BlueprintActor(
		TEXT("/AdventureTools/PointAndClick/Blueprints/Widgets/BP_ItemSlot.BP_ItemSlot_C"));
	static ConstructorHelpers::FObjectFinder<UItem> ItemInstance(
		TEXT("/AdventureTools/PointAndClick/Blueprints/Inventory/Items/Test_Item.Test_Item"));

	
	if (BlueprintActor.Succeeded())
	{
		ButtonClass = BlueprintActor.Class;
	}
	if (ItemInstance.Succeeded())
	{
		Item = ItemInstance.Object;
	}
}

UItemSlot* UButtonFixture::CreateItemSlotButton(UWorld* World)
{
	Button = CreateWidget<UItemSlot>(World, ButtonClass);

	Button->AddItem(Item);
	
	return Button;
}
