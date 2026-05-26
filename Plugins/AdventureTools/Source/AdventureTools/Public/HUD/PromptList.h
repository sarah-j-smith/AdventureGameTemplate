// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "DialogPrompt.h"
#include "PromptDisplayData.h"
#include "Blueprint/UserWidget.h"

#include "DialogComponent.h"
#include "PromptList.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTURETOOLS_API UPromptList : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeOnInitialized() override;

public:
    FOnPromptClickedDelegate PromptClickedEvent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "PromptList")
    UDialogPrompt *Prompt1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "PromptList")
    UDialogPrompt *Prompt2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "PromptList")
    UDialogPrompt *Prompt3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "PromptList")
    UDialogPrompt *Prompt4;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "PromptList")
    UDialogPrompt *Prompt5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "PromptList")
    TArray<UDialogPrompt*> PromptEntries;

    UFUNCTION(BlueprintCallable, Category = "PromptList")
    void HidePromptEntries();

    void SetPromptMenusEnabled(bool Enabled);
    
    UFUNCTION()
    void DisplayPrompts(const FPromptDisplayData& PromptDisplayData);

    UFUNCTION(BlueprintCallable, Category = "PromptList")
    void SetPromptText(FText TextToBark, bool HasBeenSelected, uint8 RowIndex);

private:
    UFUNCTION()
    void OnPrompt1Clicked();
    
    UFUNCTION()
    void OnPrompt2Clicked();
    
    UFUNCTION()
    void OnPrompt3Clicked();
    
    UFUNCTION()
    void OnPrompt4Clicked();
    
    UFUNCTION()
    void OnPrompt5Clicked();
};
