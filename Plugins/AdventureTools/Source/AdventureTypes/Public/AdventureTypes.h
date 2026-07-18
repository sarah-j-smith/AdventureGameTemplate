#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAdventureTypes, Verbose, All);

class FAdventureTypesModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    void RegisterFactories();
    
private:
    FDelegateHandle RegisterFactoriesHandle;
};
