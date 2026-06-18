#include "AdventureTypes.h"
#include "AdventureTypesStyles.h"

#define LOCTEXT_NAMESPACE "FAdventureTypesModule"

void FAdventureTypesModule::StartupModule()
{
    FAdventureTypesStyles::Get().SetUp();
}

void FAdventureTypesModule::ShutdownModule()
{
    FAdventureTypesStyles::Get().TearDown();
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FAdventureTypesModule, AdventureTypes)