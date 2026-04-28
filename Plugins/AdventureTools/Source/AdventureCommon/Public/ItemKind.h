// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "ItemKind.generated.h"

UENUM(BlueprintType)
enum class EItemKind: uint8
{
    None = 0 UMETA(DisplayName = "NONE"),
    Pickle = 1 UMETA(DisplayName = "PICKLE"),
    PickleKey = 2 UMETA(DisplayName = "PICKLE KEY"),
    Knife = 3 UMETA(DisplayName = "KNIFE"),
};

struct ADVENTURECOMMON_API FItemKind
{
    /// A string describing a list of items. Mostly for debug purposes.
    static FString GetListDescription(const TArray<EItemKind>& List);

    /// Text for a generic item of that kind, used as a fallback to show to
    /// players, i18n via strings file. Generally overridden in a blueprint.
    static FText GetDescription(const EItemKind& ItemKind);

    /// Name that is unique among all items, not translated/shown to player,
    /// used to locate the item in the data tables.
    static FName GetUniqueName(const EItemKind& ItemKind);
};
