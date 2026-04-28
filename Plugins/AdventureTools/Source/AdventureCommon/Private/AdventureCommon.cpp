#include "AdventureCommon.h"

#define LOCTEXT_NAMESPACE "FAdventureCommonModule"

DEFINE_LOG_CATEGORY(LogAdventureCommon);

void FAdventureCommonModule::StartupModule()
{
    UE_LOG(LogAdventureCommon, Verbose, TEXT("FAdventureCommonModule::StartupModule()"));
}

void FAdventureCommonModule::ShutdownModule()
{
    UE_LOG(LogAdventureCommon, Verbose, TEXT("FAdventureCommonModule::ShutdownModule()"));
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FAdventureCommonModule, AdventureCommon)