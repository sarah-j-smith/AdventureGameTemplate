// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"

#include "Constants.h"
#include "BarkRequest.h"

#include "Blueprint/UserWidget.h"

#include "BarkText.generated.h"

enum class EBarkRequestFinishedReason : uint8;

class IPositionProvider;
class UBarkLine;
class UVerticalBox;
class USphereComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FBarkRequestComplete, int32 /* UID */);
DECLARE_MULTICAST_DELEGATE_OneParam(FBarkRequestInterrupted, int32 /* UID */);

/**
 * 
 */
UCLASS()
class DIALOG_API UBarkText : public UUserWidget
{
	GENERATED_BODY()
protected:
		
	//////////////////////////////////
	///
	/// EVENTS
	///

	virtual void NativeOnInitialized() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void NativeDestruct() override;

public:
	
	void OnUserInteracted();

	//////////////////////////////////
	///
	/// BARK HANDLING
	///

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BarkText")
	TSubclassOf<UBarkLine> BarkLineClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "BarkText")
	UVerticalBox *BarkContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BarkText")
	float BarkLineDisplayTime = BARK_LINE_DELAY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BarkText")
	int32 MaxCharactersPerLine = BARK_LINE_WIDTH;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BarkText")
	FColor BarkTextColor = FColor::Orange;

	FBarkRequestComplete BarkRequestCompleteDelegate;
	FBarkRequestInterrupted BarkRequestInterruptedDelegate;
	
	void SetPositionProvider(IPositionProvider *APositionProvider);
	
	void AddBarkRequest(const FBarkRequest *BarkRequest);
	
	/// Clear all the lines of text from the widget
	void ClearText();

	bool IsBarking() const { return bIsBarking; }

	bool IsPlayerRequest() const;

	float OverrideDisplayTime = INVALID_BARK_DELAY;
	
private:

	UPROPERTY()
	USphereComponent* BarkPosition;

	IPositionProvider *PositionProvider;
	
	//////////////////////////////////
	///
	/// BARK CONTAINER
	///

	/// Helper method to set a single line of text into the bark container
	void SetText(const FText &NewText);

	void HideContainer();

	void ShowContainer();

	/// Show a warning if we can't get the view target
	bool bWarningShown = false;

	bool IsHidden = false;

	UPROPERTY()
	AActor *ViewTarget = nullptr;
		
	//////////////////////////////////
	///
	/// BARK LINES MANAGEMENT
	///
	
	/// Cheap timer updated by the tick function, since we already need tick
	float BarkLineTimer;

	/// True when the timer is running and barks are displaying
	bool bIsBarking = false;
	
	/// Set an array of lines of text into the bark container. First the bark container
	/// is cleared, the timer reset, and the first line pushed into the container.
	/// Then after every timeout another line is added.
	void SetBarkLines(const TArray<FText> &NewBarkLines);

	/// Which line in the BarkLines array is being shown
	int CurrentBarkLine = 0;

	/// A set of lines that is being scrolled into view in the container
	TArray<FText> BarkLines;
	
	void SetBarkLineTimer();

	void ClearBarkLineTimer();

	void DumpBarkText();

		
	//////////////////////////////////
	///
	/// BARK REQUESTS
	///
	
	void LoadNextBarkRequest();

	void AddToLinkedList(const FBarkRequest *BarkRequest);

	FBarkRequest *PopBarkRequest();

	FBarkRequest *RequestQueue;

	const FBarkRequest *CurrentBarkRequest;
	
	int32 CurrentUID = -1;

	bool IsRenderTransitionSet = false;

	bool IsOneLineAtMinimumSet = false;
	
	/// Called when the BarkLineTimer times out. Keeps feeding the current multiline
	/// message <code>BarkLines</code> into the bark container; or if that is complete
	/// then loads a BarkRequest if one is queued.
	void AddQueuedBarkLine(EBarkRequestFinishedReason Reason);

public:
	/// Remove all queued requests
	void ClearBarkQueue();
	
	/// Testing only
	void DoTick(float DeltaTime);
	
	/// Testing only
	float ElapsedTime() const;
};

inline FBarkRequest *UBarkText::PopBarkRequest()
{
	if (RequestQueue)
	{
		FBarkRequest *BarkRequest = RequestQueue;
		RequestQueue = RequestQueue->NextRequest;
		return BarkRequest;
	}
	return nullptr;
}

inline void UBarkText::ClearBarkQueue()
{
	bIsBarking = false;
	while (RequestQueue)
	{
		const FBarkRequest *Request = RequestQueue;
		RequestQueue = RequestQueue->NextRequest;
		if (Request == CurrentBarkRequest)
		{
			CurrentBarkRequest = nullptr;
		}
		delete Request;
	}
	if (CurrentBarkRequest != nullptr)
	{
		delete CurrentBarkRequest;
		CurrentBarkRequest = nullptr;
	}
}

inline void UBarkText::AddToLinkedList(const FBarkRequest *BarkRequest)
{
	if (RequestQueue)
	{
		FBarkRequest *Tail = RequestQueue;
		for ( ; Tail->NextRequest; Tail = Tail->NextRequest ) {}
		Tail->NextRequest = const_cast<FBarkRequest*>(BarkRequest);
	}
	else
	{
		RequestQueue = const_cast<FBarkRequest*>(BarkRequest);
	}
}