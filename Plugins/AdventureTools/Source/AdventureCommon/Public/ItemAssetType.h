#pragma once


#include "CoreMinimal.h"

#include "ItemAssetType.generated.h"

UENUM(BlueprintType)
enum class EItemAssetType: uint8
{
    /// Plain object that has no purpose apart from being possessed. Usually
    /// can be examined, and may be given to someone.
    Article     = 0    UMETA(DisplayName = "Article"),
    
    /// After being successfully used it is destroyed
    Consumable  = 1    UMETA(DisplayName = "Consumable"),

    /// Can be used <b>on</b> another item to create a brand new item.
    /// Note that a tool cannot be a consumable, so if the item needs
    /// to be destroyed after the operation, create a custom blueprint.
    Tool        = 2    UMETA(DisplayName = "Tool"),

    /// Can be used <b>on</b> a hotspot
    Key         = 3    UMETA(DisplayName = "Key"),
};
