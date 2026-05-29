// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "VerbType.h"
#include "Styling/SlateTypes.h"

#include "VerbsUI.generated.h"

class UCommonButtonBase;
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnVerbChanged, EVerbType, CurrentVerb);

/**
 * 
 */
UCLASS()
class ADVENTURETOOLS_API UVerbsUI : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized() override;

public:
	/// Handle this event to be notified when the verb changes. The argument passed is
	/// the new current verb.
	FOnVerbChanged OnVerbChanged;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category="Button Controls")
	UCommonButtonBase *C_Give;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category="Button Controls")
	UCommonButtonBase *C_Open;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category="Button Controls")
	// UButton *Close;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category="Button Controls")
	UCommonButtonBase *C_Close;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category="Button Controls")
	UCommonButtonBase *C_PickUp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category="Button Controls")
	UCommonButtonBase *C_LookAt;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category="Button Controls")
	UCommonButtonBase *C_TalkTo;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category="Button Controls")
	UCommonButtonBase *C_Use;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category="Button Controls")
	UCommonButtonBase *C_Push;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category="Button Controls")
	UCommonButtonBase *C_Pull;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Button Controls")
	EVerbType CurrentVerb;

	////// 
	/// Button handling
	///

	UFUNCTION() void CloseTriggered();
	UFUNCTION() void OpenTriggered();
	UFUNCTION() void GiveTriggered();
	UFUNCTION() void TalkToTriggered();
	UFUNCTION() void LookAtTriggered();
	UFUNCTION() void PickUpTriggered();
	UFUNCTION() void UseTriggered();
	UFUNCTION() void PushTriggered();
	UFUNCTION() void PullTriggered();
	
	void ClearActiveButton();
	
private:
	void SetButtonActive(EVerbType VerbType);

	void SetActiveVerb() const;
};
