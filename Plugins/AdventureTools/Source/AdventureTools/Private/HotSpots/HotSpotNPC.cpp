// (c) 2025 Sarah Smith


#include "HotSpots/HotSpotNPC.h"
#include "Components/SphereComponent.h"
#include "DialogComponent.h"
#include "PaperFlipbookComponent.h"
#include "Player/AdventurePlayerController.h"
#include "HUD/AdventureGameHUD.h"
#include "HUD/PromptList.h"

// Sets default values
AHotSpotNPC::AHotSpotNPC()
{
    PrimaryActorTick.bCanEverTick = true;

    // This is not a "real" sphere - its not a mesh, its just a collision volume defined by dimensions
    BarkPosition = CreateDefaultSubobject<USphereComponent>(TEXT("BarkPosition"));
    BarkPosition->SetupAttachment(RootComponent);
    BarkPosition->SetSphereRadius(4.0f);

    FlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>("FlipbookComponent");
    FlipbookComponent->SetupAttachment(RootComponent);
    FlipbookComponent->SetLooping(true);
    FlipbookComponent->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));

    DialogComponent = CreateDefaultSubobject<UDialogComponent>("DialogComponent");

    if (DefaultFlipbook)
    {
        FlipbookComponent->SetFlipbook(DefaultFlipbook);
    }
}

EVerbType AHotSpotNPC::CheckForDefaultCommand() const
{
    if (DialogComponent->ConversationCount() > 0)
    {
        return EVerbType::TalkTo;
    }
    return Super::CheckForDefaultCommand();
}

// Called when the game starts or when spawned
void AHotSpotNPC::BeginPlay()
{
    Super::BeginPlay();

    DialogComponent->ConversationEndedEvent.AddUObject(this, &AHotSpotNPC::OnConversationComplete);
}

// Called every frame
void AHotSpotNPC::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (IsConversing && ShouldStopConversation)
    {
        ShouldStopConversation = false;
        StopConversation();
    }
}

void AHotSpotNPC::OnConverseWith_Implementation()
{
    IDialogInteractable::OnConverseWith_Implementation();
    if (ACommandManager *Command = GetCommandManager())
    {
        Command->CommenceConversation();
    }
    if (UAdventureGameInstance* GameInstance = Cast<UAdventureGameInstance>(GetGameInstance()))
    {
        if (const UAdventureGameHUD *HUD = GameInstance->GetHUD())
        {
            HUD->PromptList->PromptClickedEvent.AddDynamic(DialogComponent, &UDialogComponent::HandlePromptClick);
            DialogComponent->HandleConversations(HUD->Bark);
            IsConversing = true;
        }
    }
    // Could not fetch game instance or HUD
    verify(IsConversing);
}

void AHotSpotNPC::OnTalkTo_Implementation()
{
    Execute_OnConverseWith(this);
}

void AHotSpotNPC::OnConversationComplete()
{
    UE_LOG(LogTemp, Warning, TEXT("OnConversationComplete"));
    if (UAdventureGameInstance* GameInstance = Cast<UAdventureGameInstance>(GetGameInstance()))
    {
        if (const UAdventureGameHUD *HUD = GameInstance->GetHUD())
        {
            HUD->PromptList->PromptClickedEvent.RemoveAll(DialogComponent);
        }
    }
    ShouldStopConversation = true;
}

USphereComponent* AHotSpotNPC::GetPosition() const
{
    return BarkPosition;
}

void AHotSpotNPC::StopConversation()
{
    UE_LOG(LogTemp, Warning, TEXT("StopConversation - IsConversing: %s"), IsConversing ? TEXT("Yes") : TEXT("No"));
    if (!IsConversing) return;
    UE_LOG(LogTemp, Warning, TEXT("Stopping Conversation"));
    if (ACommandManager *Command = GetCommandManager())
    {
        Command->EndConversation();
    }
    IsConversing = false;
}
