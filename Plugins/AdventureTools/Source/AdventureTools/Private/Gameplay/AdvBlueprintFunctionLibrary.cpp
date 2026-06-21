// (c) 2025 Sarah Smith

#include "Gameplay/AdvBlueprintFunctionLibrary.h"

#include "Gameplay/AdventureGameInstance.h"
#include "Gameplay/AdventureGameModeBase.h"
#include "Player/CommandManager.h"
#include "Player/AdventurePlayerController.h"
#include "Player/ItemManager.h"
#include "Player/AdventureCharacter.h"
#include "Gameplay/AdventureControllerProvider.h"
#include "HotSpots/HotSpot.h"

#include "AdventureTools.h"
#include "GameUtils.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Gameplay/ManagerProvider.h"
#include "Gameplay/BarkProvider.h"

UManagerProvider* UAdvBlueprintFunctionLibrary::GetManagerProvider()
{
    static UManagerProvider *ManagerProvider = NewObject<UManagerProvider>();
    return ManagerProvider;
}

UBarkProvider* UAdvBlueprintFunctionLibrary::GetBarkProvider()
{
    static UBarkProvider *BarkProvider = NewObject<UBarkProvider>();
    return BarkProvider;
}

UAdventureControllerProvider* UAdvBlueprintFunctionLibrary::GetControllerProvider()
{
    static UAdventureControllerProvider *ControllerProvider = NewObject<UAdventureControllerProvider>();
    return ControllerProvider;
}


void UAdvBlueprintFunctionLibrary::AddToScore(const UObject* WorldContextObject, int32 ScoreIncrement)
{
    if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject,
                                                             EGetWorldErrorMode::LogAndReturnNull))
    {
        AGameModeBase *GameMode = UGameplayStatics::GetGameMode(WorldContextObject);
        if (AAdventureGameModeBase *AdventureGameMode = Cast<AAdventureGameModeBase>(GameMode))
        {
            AdventureGameMode->AddToScore(ScoreIncrement);
        }
    }
}

AAdventurePlayerController* UAdvBlueprintFunctionLibrary::GetAdventureController(UObject* WorldContextObject)
{
    return GetControllerProvider()->GetAdventurePlayerController(WorldContextObject);
}

ACommandManager* UAdvBlueprintFunctionLibrary::GetCommandManager(UObject* WorldContextObject)
{
    return GetManagerProvider()->GetCommandManager(WorldContextObject);
}

UItemManager* UAdvBlueprintFunctionLibrary::GetItemManager(UObject* WorldContextObject)
{
    return GetManagerProvider()->GetItemManager(WorldContextObject);
}

void UAdvBlueprintFunctionLibrary::PlayerBark(UObject* WorldContextObject, FText BarkText)
{
    if (UBarkProvider *BarkProvider = UAdvBlueprintFunctionLibrary::GetBarkProvider())
    {
        BarkProvider->Bark(BarkText, WorldContextObject);
    }
}

void UAdvBlueprintFunctionLibrary::ClearVerb(UObject* WorldContextObject)
{
    if (ACommandManager *CommandManager = GetCommandManager(WorldContextObject))
    {
        CommandManager->InterruptCurrentAction();
    }
}

int32 UAdvBlueprintFunctionLibrary::PIEInstance(const UObject* WorldContextObject)
{
    // https://blog.jamesbrooks.net/posts/pieinstance-in-unreal/
    const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

    // If we are in the editor, return the PIE instance ID, otherwise return -1 (not in PIE and we won't be using this value anyway)
    if (World->IsEditorWorld())
    {
        // Get world context
        if (const FWorldContext* WorldContext = GEngine->GetWorldContextFromWorld(World))
        {
            return WorldContext->PIEInstance;
        }
    }
    return -1;
}

void UAdvBlueprintFunctionLibrary::AddToInventory(UObject* WorldContextObject,
                                                             const FName ItemToAdd)
{
    if (UItemManager *ItemManager = GetItemManager(WorldContextObject))
    {
        ItemManager->ItemAddToInventory(ItemToAdd);
    }
}

void UAdvBlueprintFunctionLibrary::RemoveFromInventory(UObject* WorldContextObject, FName ItemToRemove)
{
    if (UItemManager *ItemManager = GetItemManager(WorldContextObject))
    {
        ItemManager->ItemRemoveFromInventoryAsync(ItemToRemove);
    }
}

FString UAdvBlueprintFunctionLibrary::GetProjectVersion()
{
    FString ProjectVersion;

    GConfig->GetString(
        TEXT("/Script/EngineSettings.GeneralProjectSettings"),
        TEXT("ProjectVersion"),
        ProjectVersion,
        GGameIni
    );

    return ProjectVersion;
}

float UAdvBlueprintFunctionLibrary::GetBarkTime(const FString& BarkText)
{
    return FGameUtils::GetDisplayTimeForString(BarkText);
}

UAdventureGameInstance* UAdvBlueprintFunctionLibrary::GetAdventureInstance(const UObject* WorldContextObject)
{
    if (UAdventureGameInstance* AdventureGameInstance = Cast<UAdventureGameInstance>(
        UGameplayStatics::GetGameInstance(WorldContextObject)))
    {
        return AdventureGameInstance;
    }
    UE_LOG(LogAdventureGame, Error, TEXT("Could not access Adventure Game Instance"));
    return nullptr;
}

bool UAdvBlueprintFunctionLibrary::IsCharacterCloseToHotSpot(AHotSpot* HotSpot, AAdventureCharacter* AdventureCharacter,
                                                             float Tolerance)
{
    if (HotSpot == nullptr)
    {
        UE_LOG(LogAdventureGame, Error, TEXT("HotSpot == nullptr"));
        return false;
    }
    return IsCharacterCloseToLocation(HotSpot->WalkToPosition, AdventureCharacter, Tolerance);
}

bool UAdvBlueprintFunctionLibrary::IsCharacterCloseToLocation(FVector Location, AAdventureCharacter* AdventureCharacter,
    float Tolerance)
{
    if (Tolerance <= std::numeric_limits<float>::epsilon())
    {
        UE_LOG(LogAdventureGame, Error, TEXT("Tolerance is too small - %f"), Tolerance);
        return false;
    }
    return GetCharacterDistanceToLocation(Location, AdventureCharacter) < Tolerance;
}

float UAdvBlueprintFunctionLibrary::GetCharacterDistanceToLocation(FVector Location, AAdventureCharacter* AdventureCharacter)
{
    if (AdventureCharacter == nullptr)
    {
        UE_LOG(LogAdventureGame, Error, TEXT("%hs: AdventureCharacter == nullptr"), __FUNCTION__);
        return std::numeric_limits<float>::infinity();
    }
    if (UCapsuleComponent *Capsule = AdventureCharacter->GetCapsuleComponent())
    {
        FVector WorldLocation = Capsule->GetComponentLocation();
        if (WorldLocation != FVector::ZeroVector)
        {
            WorldLocation.Z = Location.Z; // Ignore the Z, only test X & Y.
            return FVector::Distance(Location, WorldLocation);
        }
        UE_LOG(LogAdventureGame, Warning, TEXT("%hs: Player position was (0, 0, 0)!"), __FUNCTION__);
    }
    else
    {
        UE_LOG(LogAdventureGame, Warning, TEXT("%hs: Player capsule component null"), __FUNCTION__);
    }
    return std::numeric_limits<float>::infinity();
}
