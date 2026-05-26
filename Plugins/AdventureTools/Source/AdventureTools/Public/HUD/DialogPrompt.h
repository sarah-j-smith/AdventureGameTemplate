// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Engine/TimerHandle.h"

#include "DialogPrompt.generated.h"

class UImage;

/**
 * 
 */
UCLASS()
class ADVENTURETOOLS_API UDialogPrompt : public UUserWidget
{
    GENERATED_BODY()
    
protected:
    virtual void NativeOnInitialized() override;
    
public:
    /// The bullet at the start of each line
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Prompt")
    UImage *PromptBullet;

    /// The button to activate the prompt
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Prompt")
    UButton *PromptButton;

    /// The text of the button
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Prompt")
    UTextBlock *PromptText;

    UFUNCTION(BlueprintCallable, Category = "Prompt")
    void SetText(const FText &TextToSet);

    UFUNCTION(BlueprintCallable, Category = "Prompt")
    void HighlightText();

    UFUNCTION(BlueprintCallable, Category = "Prompt")
    void UnhighlightText();

    UFUNCTION(BlueprintCallable, Category = "Prompt")
    void HidePrompt();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prompt")
    bool HasBeenUsed;
    
    UFUNCTION()
    void HandleOnHover();
    
    UFUNCTION()
    void HandleOnUnhover();

    void FlashPrompt();
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prompt")
    float FlashTime = 0.6f;
    
    UFUNCTION(BlueprintCallable, Category = "Prompt")
    void PromptTimerTimeout();
    
    FTimerHandle PromptTimerHandle;
};
