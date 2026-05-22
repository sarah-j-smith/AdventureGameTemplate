// (c) 2025 Sarah Smith

#include "DialogComponent.h"

#include "BarkText.h"
#include "ConversationData.h"
#include "PositionProvider.h"

// Sets default values for this component's properties
UDialogComponent::UDialogComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UDialogComponent::BeginPlay()
{
    Super::BeginPlay();

    FillConversationData();
    ConversationDataLoad.ExecuteIfBound(this);
}

void UDialogComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    ConversationDataSave.ExecuteIfBound(this);
}

// Called every frame
void UDialogComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (NextAction != NoAction)
    {
        switch (NextAction)
        {
        case ShowNPCResponseAction:
            ShowNPCResponse();
            break;
        case ShowNextDialogPromptAction:
            ShowNextDialogPrompts();
            break;
        default:
            break;
        }
        NextAction = NoAction;
    }
}

void UDialogComponent::FillConversationData()
{
    if (ConversationData.Num() > 0) return; // already filled
    for (const auto TopicTable : TopicList)
    {
        TArray<FPromptData> PromptsForTopic;
        TArray<FPromptData*> PromptPtrs;
        TopicTable->GetAllRows<FPromptData>("FillConversationData", PromptPtrs);
        for (const FPromptData* PromptData : PromptPtrs)
        {
            if (PromptData)
            {
                PromptsForTopic.Add(*PromptData);
            }
        }
        FConversationData TopicConversationData;
        TopicConversationData.ConversationPromptArray = PromptsForTopic;
        ConversationData.Add(TopicConversationData);
#if WITH_EDITOR
        FString ErrorMessage;
        bool IsOK = TopicConversationData.Validate(ErrorMessage);
        if (!IsOK && !ErrorMessage.IsEmpty())
        {
            GEngine->AddOnScreenDebugMessage(DIALOGUE_FILL_KEY, 10.0, FColor::Red,
                    *ErrorMessage,false, FVector2D(2.0, 2.0));
            UE_LOG(LogDialog, Error, TEXT("Error in Conversation: check tables: %s"), *ErrorMessage);
        }
#endif
    }
}

void UDialogComponent::UpdatePromptAtIndex(int32 ATopicIndex, int32 APromptIndex)
{
    const int PromptSubIndex = PromptsToShow[APromptIndex].PromptSubNumber;
    ConversationData[ATopicIndex].MarkPromptSelected(APromptIndex, PromptSubIndex);
}

void UDialogComponent::LoadPrompts(TArray<FPromptData>& TPromptsToShow)
{
    if (ConversationData.IsEmpty()) return;
    if (TopicIndex < ConversationData.Num() && TopicIndex >= 0)
    {
        ConversationData[TopicIndex].DisplayPrompts(TPromptsToShow);
    }
    else
    {
        UE_LOG(LogDialog, Warning,
               TEXT("TopicIndex %d is not in the bounds of conversation topics, 0 to %d"),
               TopicIndex, ConversationData.Num());
    }
}

int UDialogComponent::ConversationCount() const
{
    int Count = 0;
    for (FConversationData Element : ConversationData)
    {
        if (Element.PromptsAvailableCount() > 0)
        {
            ++Count;
        }
    }
    return Count;
}

void UDialogComponent::HandleConversations(UBarkText *ABark)
{
    InitialiseForBarking(ABark);
    // Get the prompts and display them on the buttons, wait for the player to click
    PromptsToShow.Empty();
    LoadPrompts(PromptsToShow);
    DisplayPrompts();
}

void UDialogComponent::StopMonitoringConversations()
{
    ConversationEndedEvent.Broadcast();
    TearDownBarkingSetup();
    PromptsToShow.Empty();
    DialogState = EDialogState::Hidden;
}

void UDialogComponent::AssignNewTopic(const UDataTable* NewTopic)
{
    int IndexToSet = 0;
    if (!IsValid(NewTopic))
    {
        UE_LOG(LogDialog, Warning, TEXT("New topic is NULL"));
        return;
    }
    for (const UDataTable* Topic : TopicList)
    {
        // Find this topic table in our list, so we know the index
        if (Topic == NewTopic)
        {
            TopicIndex = IndexToSet;
            UE_LOG(LogDialog, Warning, TEXT("Assigning new conversation topic - %d"), TopicIndex);
            return;
        }
        ++IndexToSet;
    }
    // Got a topic that weirdly was not in our list - should never happen
    UE_LOG(LogDialog, Error, TEXT("UDialogComponent::AssignNewTopic - Got bad topic"));
}

void UDialogComponent::HandlePromptClick(int PromptIndex)
{
    CurrentPromptIndex = PromptIndex - 1; // Prompts are numbered 1 - 5
    UpdatePromptAtIndex(TopicIndex, CurrentPromptIndex);
    ShowPlayerBark();
}

void UDialogComponent::DisplayPrompts()
{
    UE_LOG(LogDialog, Warning, TEXT("DisplayPrompts"));
    const int PromptCount = PromptsToShow.Num();
    FPromptDisplayData PromptDisplayData;
    for (int PromptIndex = 0; PromptIndex < PromptCount; ++PromptIndex)
    {
        PromptDisplayData.Units.Add(PromptsToShow[PromptIndex].GetDisplayData());
    }
    OnDisplayPrompts.Broadcast(PromptDisplayData);
    DialogState = EDialogState::Prompts;
}

void UDialogComponent::ShowPlayerBark()
{
    DialogState = EDialogState::Player;
    const FBarkRequest* PlayerRequest = FBarkRequest::CreatePlayerMultilineRequest(
        PromptsToShow[CurrentPromptIndex].PromptText);
    BarkUID = PlayerRequest->GetUID();
    Bark->AddBarkRequest(PlayerRequest);
    UE_LOG(LogDialog, Warning, TEXT("UDialogComponent::ShowPlayerBark - UID: %d - %s"), BarkUID,
        *(PromptsToShow[CurrentPromptIndex].PromptText[0].ToString()));
}

void UDialogComponent::ShowNPCResponse()
{
    UE_LOG(LogDialog, Warning, TEXT("ShowNPCResponse"));
    DialogState = EDialogState::NPC;
    const IPositionProvider* ANPC = Cast<IPositionProvider>(GetOwner());
    check(ANPC);
    const FBarkRequest* NPCRequest = FBarkRequest::CreateNPCMultilineRequest(
        PromptsToShow[CurrentPromptIndex].NPCResponse, 0,
        ANPC->GetPosition(), TextColor.ToFColor(true));
    BarkUID = NPCRequest->GetUID();
    Bark->AddBarkRequest(NPCRequest);
}

void UDialogComponent::ShowNextDialogPrompts()
{
    UE_LOG(LogDialog, Warning, TEXT("ShowNextDialogPrompts - current prompt: %d - %s"), CurrentPromptIndex,
        *(PromptsToShow[CurrentPromptIndex].PromptText[0].ToString()));
    if (PromptsToShow[CurrentPromptIndex].EndsConversation)
    {
        UE_LOG(LogDialog, Warning, TEXT("EndsConversation"));
        if (!PopConversationTopic())
        {
            UE_LOG(LogDialog, Warning, TEXT("EndsConversation - exiting"));
            StopMonitoringConversations();
            return;
        }
    }
    else if (PromptsToShow[CurrentPromptIndex].SwitchTopic)
    {
        UE_LOG(LogDialog, Warning, TEXT("Not: EndsConversation - SwitchTopic"));
        PushConversationTopic();
    }
    PromptsToShow.Empty();
    LoadPrompts(PromptsToShow);
    DisplayPrompts();
}

void UDialogComponent::InitialiseForBarking(UBarkText *ABark)
{
    Bark = ABark;
    ABark->BarkRequestCompleteDelegate.AddUObject(this, &UDialogComponent::OnBarkTimerTimeout);
    ABark->BarkRequestInterruptedDelegate.AddUObject(this, &UDialogComponent::OnBarkTimerTimeout);
}

void UDialogComponent::TearDownBarkingSetup()
{
    if (Bark)
    {
        Bark->BarkRequestCompleteDelegate.RemoveAll(this);
        Bark->BarkRequestInterruptedDelegate.RemoveAll(this);
    }
    BarkUID = BARK_UID_NONE;
    Bark = nullptr;
}

void UDialogComponent::OnBarkTimerTimeout(int32 UID)
{
    // Triggered by the Player Controller's Bark Timeout.
    UE_LOG(LogDialog, VeryVerbose, TEXT("OnBarkTimerTimeout: Expected UID: %d vs Got UID: %d"),
           BarkUID, UID);
    if (UID == BarkUID)
    {
        // Only react if its our text that caused the timeout
        if (DialogState == EDialogState::Player)
        {
            NextAction = ShowNPCResponseAction;
        }
        else if (DialogState == EDialogState::NPC)
        {
            NextAction = ShowNextDialogPromptAction;
        }
    }
}

bool UDialogComponent::PopConversationTopic()
{
    if (Stack.Num() > 0)
    {
        TopicIndex = Stack.Pop();
        return true;
    }
    return false;
}

void UDialogComponent::PushConversationTopic()
{
    UE_LOG(LogDialog, Warning, TEXT("PushConversationTopic"));
    Stack.Push(TopicIndex);
    const UDataTable* NewTopic = PromptsToShow[CurrentPromptIndex].SwitchTopic;
    AssignNewTopic(NewTopic);
}
