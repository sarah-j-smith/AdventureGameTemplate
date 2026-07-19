// (c) 2025 Sarah Smith


#include "BarkText.h"

#include "BarkLine.h"
#include "BarkRequest.h"
#include "BarkRequestFinishReason.h"
#include "Dialog.h"
#include "PositionProvider.h"

#include "Components/SphereComponent.h"

#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"

void UBarkText::NativeOnInitialized()
{
    UE_LOG(LogDialog, VeryVerbose, TEXT("BarkText Initialized"));
    HideContainer();
    bIsBarking = false;
    ViewTarget = nullptr;
}

bool UBarkText::IsPlayerRequest() const
{
    return CurrentBarkRequest && CurrentBarkRequest->IsPlayer();
}


void UBarkText::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (bIsBarking && IsRenderTransitionSet && IsOneLineAtMinimumSet)
    {
        ShowContainer();
    }

    if (bIsBarking)
    {
        if (ViewTarget == nullptr)
        {
            const APlayerController* PlayerController = GetOwningPlayer();
            ViewTarget = PlayerController->GetViewTarget();
        }
        if (ViewTarget)
        {
            const FVector CameraLocation = ViewTarget->GetActorLocation();
            const FVector Offset = BarkPosition->GetComponentLocation() - CameraLocation;
            SetRenderTranslation(FVector2D(Offset.X, Offset.Y));
            IsRenderTransitionSet = true;
        }
    }

    if (bIsBarking)
    {
        BarkLineTimer -= InDeltaTime;
        if (BarkLineTimer <= 0.f)
        {
            AddQueuedBarkLine(EBarkRequestFinishedReason::Timeout);
        }
    }
}

void UBarkText::NativeDestruct()
{
    ClearBarkQueue();
    Super::NativeDestruct();
}

void UBarkText::OnUserInteracted()
{
    if (bIsBarking)
    {
        BarkLineTimer = 0.0f;
        AddQueuedBarkLine(EBarkRequestFinishedReason::Interruption);
    }
}

void UBarkText::SetPositionProvider(IPositionProvider* APositionProvider)
{
    PositionProvider = APositionProvider;
}

void UBarkText::AddBarkRequest(const FBarkRequest *BarkRequest)
{
    UE_LOG(LogDialog, Warning, TEXT("AddBarkRequest"));
    FBarkRequest::Dump(const_cast<FBarkRequest *>(BarkRequest));
    AddToLinkedList(BarkRequest);
    if (!bIsBarking)
    {
        LoadNextBarkRequest();
        bIsBarking = true;
        BarkLineTimer = 0.1f;
    }
}

void UBarkText::SetText(const FText &NewText)
{
    UE_LOG(LogDialog, VeryVerbose, TEXT(">> ============== SetText ================"));
    APlayerController* PlayerController = GetOwningPlayer();
    check(PlayerController);
    check(BarkLineClass);
    DumpBarkText();
    if (BarkContainer->GetChildrenCount() >= G_MAX_BARK_LINES)
    {
        UWidget *OldChild = BarkContainer->GetChildAt(0);
        BarkContainer->ShiftChild(2, OldChild);
        UBarkLine *BarkLine = Cast<UBarkLine>(OldChild);
        BarkLine->Text->SetText(NewText);
        return;
    }
    UUserWidget* Widget = CreateWidget(PlayerController, BarkLineClass);
    if (UBarkLine* BarkLine = Cast<UBarkLine>(Widget))
    {
        BarkLine->Text->SetText(NewText);
        BarkLine->Text->SetColorAndOpacity(BarkTextColor);
        BarkContainer->AddChildToVerticalBox(Widget);
    }
    IsOneLineAtMinimumSet = true;
    DumpBarkText();
    UE_LOG(LogDialog, VeryVerbose, TEXT("<< ============== SetText ================"));
}

void UBarkText::ClearText()
{
    UE_LOG(LogDialog, VeryVerbose, TEXT("ClearText"));
    HideContainer();
    ClearBarkQueue();
    ClearBarkLineTimer();
    BarkContainer->ClearChildren();
    BarkLines.Empty();
    bWarningShown = false;
    CurrentUID = BARK_UID_NONE;
    CurrentBarkLine = 0;
    IsRenderTransitionSet = false;
    IsOneLineAtMinimumSet = false;
}

void UBarkText::HideContainer()
{
    if (IsHidden) return;
    UE_LOG(LogDialog, VeryVerbose, TEXT("HideContainer"));
    BarkContainer->SetVisibility(ESlateVisibility::Hidden);
    IsHidden = true;
}

void UBarkText::ShowContainer()
{
    if (!IsHidden) return;
    UE_LOG(LogDialog, VeryVerbose, TEXT("ShowContainer"));
    BarkContainer->SetVisibility(ESlateVisibility::Visible);
    IsHidden = false;
}

void UBarkText::SetBarkLines(const TArray<FText>& NewBarkLines)
{
    ClearText();
    if (NewBarkLines.Num() == 0) return;
    SetText(NewBarkLines[0]);
    if (NewBarkLines.Num() > 1)
    {
        auto It = NewBarkLines.begin();
        for (++It; It != NewBarkLines.end(); ++It)
        {
            const FText& Line = *It;
            BarkLines.Add(*It);
        }
        SetBarkLineTimer();
    }
}

void UBarkText::LoadNextBarkRequest()
{
    CurrentBarkRequest = PopBarkRequest();
    if (!CurrentBarkRequest)
    {
        UE_LOG(LogDialog, Warning, TEXT("Bark Request Loading Failed"));
        return;
    }
    BarkLines.Empty();
    CurrentBarkRequest->GetBarkLines(BarkLines);
    CurrentUID = CurrentBarkRequest->GetUID();
    UE_LOG(LogDialog, Verbose, TEXT("LoadNextBarkRequest: %d - %s - duration: %0.2f"), CurrentUID, 
        *BarkLines[0].ToString(), CurrentBarkRequest->GetDuration());
    BarkPosition = CurrentBarkRequest->GetPosition();
    if (!IsValid(BarkPosition))
    {
        // Invariant: any bark request with a null position is the player barking
        ensureAlwaysMsgf(CurrentBarkRequest->IsPlayer(), TEXT("All bark requests that are NPCs must have a bark position!"));
        UE_LOG(LogDialog, VeryVerbose, TEXT("BarkPosition Invalid - using player position - GetAdventureCharacter"));
        if (const IPositionProvider *HavePositionProvider = this->PositionProvider)
        {
            BarkPosition = HavePositionProvider->GetPosition();
        }
    }
    BarkLineDisplayTime = CurrentBarkRequest->GetDuration();
    if (OverrideDisplayTime != INVALID_BARK_DELAY) BarkLineDisplayTime = OverrideDisplayTime;
    BarkTextColor = CurrentBarkRequest->GetColor();
}

void UBarkText::AddQueuedBarkLine(EBarkRequestFinishedReason Reason)
{
    UE_LOG(LogDialog, VeryVerbose, TEXT("AddQueuedBarkLine - count: %d - current: %d"), BarkLines.Num(), CurrentBarkLine);
    if (CurrentBarkLine < BarkLines.Num())
    {
        HideContainer();
        IsRenderTransitionSet = false;
        IsOneLineAtMinimumSet = false;
        UE_LOG(LogDialog, VeryVerbose, TEXT("AddQueuedBarkLine: %d %s"), CurrentBarkLine, *BarkLines[CurrentBarkLine].ToString())
        SetText(BarkLines[CurrentBarkLine]);
        SetBarkLineTimer();
        CurrentBarkLine++;
    }
    else
    {
        UE_LOG(LogDialog, VeryVerbose, TEXT("AddQueuedBarkLine - no more lines - doing clean up"));
        if (CurrentBarkRequest)
        delete CurrentBarkRequest;
        CurrentBarkRequest = nullptr;
        UE_LOG(LogDialog, VeryVerbose, TEXT("AddQueuedBarkLine - UID %d, broadcasting - doing clean up"), CurrentUID);
        switch (Reason)
        {
        case EBarkRequestFinishedReason::Timeout:
            BarkRequestCompleteDelegate.Broadcast(CurrentUID);
            break;
        case EBarkRequestFinishedReason::Interruption:
            BarkRequestInterruptedDelegate.Broadcast(CurrentUID);
            break;
        case EBarkRequestFinishedReason::BarkAndEnd:
            BarkRequestCompleteDelegate.Broadcast(CurrentUID);
            break;
        }
        HideContainer();
        ClearText();
        if (RequestQueue)
        {
            LoadNextBarkRequest();
            SetBarkLineTimer();
        }
        else
        {
            UE_LOG(LogDialog, Verbose, TEXT("AddQueuedBarkLine - request queue empty stop barking"));
            bIsBarking = false;
        }
    }
}

void UBarkText::DoTick(float DeltaTime)
{
    if (GIsAutomationTesting)
    {
        FVector2D FakeSize(320.0f, 45.0f);
        FSlateLayoutTransform FakeTransform(FVector2D(100.0f, 100.0f));

        // 3. Make a geometry object using the helper constructor
        const FGeometry MockGeometry = FGeometry::MakeRoot(FakeSize, FakeTransform);
    
        NativeTick(MockGeometry, DeltaTime);
    }
    else
    {
        UE_LOG(LogTemp, Fatal, TEXT("UBarkText::DoTick - IS ONLY FOR TESTING"));
    }
}

float UBarkText::ElapsedTime() const
{
    if (GIsAutomationTesting)
    {
        return BarkLineTimer;
    }
    else
    {
        UE_LOG(LogTemp, Fatal, TEXT("UBarkText::DoTick - IS ONLY FOR TESTING"));
        return 0.0f; /// Never reached.
    }
}

FBarkRequest* UBarkText::GetCurrentBarkRequest() const
{
    if (GIsAutomationTesting)
    {
        return const_cast<FBarkRequest*>(CurrentBarkRequest);
    }
    return nullptr;
}

void UBarkText::SetBarkLineTimer()
{
    BarkLineTimer = CurrentBarkRequest ? CurrentBarkRequest->GetDurationForLine(CurrentBarkLine) : BarkLineDisplayTime;
    bIsBarking = true;
    UE_LOG(LogDialog, VeryVerbose, TEXT("#### SetBarkLineTimer: %f"), BarkLineTimer);
}

void UBarkText::ClearBarkLineTimer()
{
    BarkLineTimer = 0;
    bIsBarking = false;
}

void UBarkText::DumpBarkText()
{
    auto Children = BarkContainer->GetAllChildren();
    int i = 0;
    for (UWidget* Child : Children)
    {
        if (const auto BarkLine = Cast<UBarkLine>(Child))
        {
            FText LineText = BarkLine->Text->GetText();
            UE_LOG(LogDialog, VeryVerbose, TEXT("Widget display: %d - %s"), i, *(LineText.ToString()));
        }
        i++;
    }

    for (FText Text : BarkLines)
    {
        UE_LOG(LogDialog, VeryVerbose, TEXT("Queued Barkline: %d - %s"), i, *(Text.ToString()));
    }
}
