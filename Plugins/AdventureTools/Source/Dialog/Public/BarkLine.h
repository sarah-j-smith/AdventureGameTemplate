// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "BarkLine.generated.h"

/**
 * 
 */
UCLASS()
class DIALOG_API UBarkLine : public UUserWidget
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Bark Line")
    UTextBlock *Text;
};
