#include "AdventureTypes.h"
#include "AdventureTypesStyles.h"
#include "FollowCameraFactory.h"
#include "IPlacementModeModule.h"
#include "RoomSceneryFactory.h"

DEFINE_LOG_CATEGORY(LogAdventureTypes);

#define LOCTEXT_NAMESPACE "FAdventureTypesModule"

void FAdventureTypesModule::StartupModule()
{
    FAdventureTypesStyles::Get().SetUp();
    if (IPlacementModeModule::IsAvailable())
    {
        //IPlacementModeModule::Get().UnregisterPlacementCategory("AdventureTools");
        UE_LOG(LogAdventureTypes, Log, TEXT("FAdventureTypesModule::StartupModule IPlacementModeModule::IsAvailable()"));
        RegisterFactories();
    }
    else
    {
        FCoreDelegates::OnPostEngineInit.AddRaw(this, &FAdventureTypesModule::RegisterFactories);
        UE_LOG(LogAdventureTypes, Log, TEXT("FAdventureTypesModule::StartupModule IPlacementModeModule  NOT LOADED"));
    }
    UE_LOG(LogAdventureTypes, Log, TEXT("FAdventureTypesModule::StartupModule - loaded"));
}

void FAdventureTypesModule::ShutdownModule()
{
    FAdventureTypesStyles::Get().TearDown();
    if (IPlacementModeModule::IsAvailable())
    {
        IPlacementModeModule::Get().UnregisterPlacementCategory("AdventureTools");
    }
    if (RegisterFactoriesHandle.IsValid())
    {
        FCoreDelegates::OnPostEngineInit.Remove(RegisterFactoriesHandle);
        RegisterFactoriesHandle.Reset();
    }
}

void FAdventureTypesModule::RegisterFactories()
{
    UE_LOG(LogAdventureTypes, Log, TEXT("FAdventureTypesModule::RegisterFactories()"));
    
    if (GEditor == nullptr)
    {
        UE_LOG(LogAdventureTypes, Error, TEXT("FAdventureTypesModule::PostLoadCallback() - no editor running"));
        return;
    }
    
    // Create a custom category and register it
    const FPlacementCategoryInfo Info(
        LOCTEXT("Place_Actors_Category_Name", "Adventure Tools"),
        FSlateIcon(FAppStyle::GetAppStyleSetName(), "PlacementBrowser.Icons.Basic"),
        "AdventureTools",
        TEXT("AdventureTools"),
        100
    );
    
    IPlacementModeModule& PlacementModeModule = IPlacementModeModule::Get();
    PlacementModeModule.RegisterPlacementCategory(Info);
 
    // Add actor classes to the category
    PlacementModeModule.RegisterPlaceableItem(Info.UniqueHandle,
        MakeShared<FPlaceableItem>(*URoomSceneryFactory::StaticClass()));
    PlacementModeModule.RegisterPlaceableItem(Info.UniqueHandle,
        MakeShared<FPlaceableItem>(*UFollowCameraFactory::StaticClass()));
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FAdventureTypesModule, AdventureTypes)