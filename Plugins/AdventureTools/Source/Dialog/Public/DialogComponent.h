// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Constants.h"
#include "ConversationData.h"
#include "PromptDisplayData.h"
#include "DialogState.h"

#include "DialogComponent.generated.h"

class UBarkText;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPromptClickedDelegate, int, PromptIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDisplayPrompts, const FPromptDisplayData&, PromptData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBarkCompleted, int32, UIDOfEndingBark);

DECLARE_DYNAMIC_DELEGATE_OneParam(FConversationDataLoad, UDialogComponent *, DialogComponent);
DECLARE_DYNAMIC_DELEGATE_OneParam(FConversationDataSave, UDialogComponent *, DialogComponent);

DECLARE_MULTICAST_DELEGATE(FConversationEnded);

/**
 * Class to handle NPC speaking.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DIALOG_API UDialogComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UDialogComponent();
    
    // Copy the data out of the TopicList tables into local ConversationData arrays
    UFUNCTION(BlueprintCallable, Category = "HotSpot")
    void FillConversationData();

    // Mark this prompt has been selected by the player.
    UFUNCTION(BlueprintCallable, Category = "HotSpot")
    void UpdatePromptAtIndex(int32 ATopicIndex, int32 PromptIndex);
    
    // Called when the game starts
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    FConversationDataLoad ConversationDataLoad;
    FConversationDataSave ConversationDataSave;

    /// Respond to this event to be notified when the user selects the 
    FConversationEnded ConversationEndedEvent;
    
    FOnBarkCompleted OnBarkCompleted;
    
    FOnDisplayPrompts OnDisplayPrompts;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
    EDialogState DialogState = EDialogState::Hidden;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dialog")
    FLinearColor TextColor = G_NPC_Default_Text_Colour;

    /// Conversations loaded from data tables, ready to be barked.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
    TArray<FConversationData> ConversationData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
    int32 TopicIndex = 0;
    
    /// Data tables that contain the dialogue at design time, in the editor.
    /// The conversations will be loaded on BeginPlay
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
    TArray<UDataTable *> TopicList;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prompts")
    TArray<FPromptData> PromptsToShow;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prompts")
    int CurrentPromptIndex = 0;
    
    /**
     * Get the prompts at the current topic as an array of prompt data
     * from the data tables.
     * @param PromptsToShow Out parameter to be filled with the prompts
     */
    void LoadPrompts(TArray<FPromptData> &PromptsToShow);

    /**
     * Calculate the non-empty conversation topics that remain. This call is
     * expensive as it iterates through checking for topics with prompts that
     * can be shown.
     * @see <code>UPromptData::CanBeShown()</code>
     * @return int Count of all the conversations available
     */
    int ConversationCount() const;

    /**
     * Begin displaying conversations that are selected in the prompt list.
     * <b>Caller must connect the appropriate events</b>
     * eg FOnPromptClickedDelegate to the HandlePromptClick method.
     * @param ABark The bark text UI widget to display the prompts on.
     */
    void HandleConversations(UBarkText *ABark);

    /** 
     * Called internally by the dialog component to tear down the barking setup
     * and clear the prompts. This also broadcasts ConversationEndedEvent.
     * Its called when the logic to show the next dialog prompt finds that there
     * are no more topics to display, because the player has clicked the exit prompt
     * labelled with the <code>EndsConversation == true</code> flag.
     */
    void StopMonitoringConversations();

    /**
     * Assign a new conversation for this NPC component to speak. The 
     * prompts will be loaded when HandleConversations is called. This should
     * be the top-level topic data table of the conversation. The lower level
     * topics are assigned and loaded automatically when needed.
     * @param NewTopic Data table containing the conversation definitions
     */
    void AssignNewTopic(const UDataTable *NewTopic);

    UFUNCTION()
    void HandlePromptClick(int PromptIndex);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dialog")
    UBarkText *Bark = nullptr;

private:
    TArray<int> Stack;

    void DisplayPrompts();
    void ShowPlayerBark();
    void ShowNPCResponse();
    void ShowNextDialogPrompts();
    void InitialiseForBarking(UBarkText *ABark);
    void TearDownBarkingSetup();

    enum ENextAction
    {
        NoAction,
        ShowNPCResponseAction,
        ShowNextDialogPromptAction
    };

    ENextAction NextAction = NoAction;

    FTimerDelegate BarkTimerDelegate;
    FTimerHandle BarkTimerHandle;
    bool IsBarking = false;

    UFUNCTION()
    void OnBarkTimerTimeout(int32 UID);

    bool PopConversationTopic();

    void PushConversationTopic();

    int32 BarkUID = 0;
};
