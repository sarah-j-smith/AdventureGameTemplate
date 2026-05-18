#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FBlueprintReparenterModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterMenuExtender();
	TSharedRef<class FExtender> OnExtendContentBrowserMenu(const TArray<FAssetData>& SelectedAssets);
	void AddReparentOption(class FMenuBuilder& MenuBuilder, FAssetData SelectedAsset);
	void OnReparentBlueprint(FAssetData BlueprintAsset);
	static TSharedRef<FExtender> OnExtendContentBrowserMenu_Static(const TArray<FAssetData>& SelectedAssets);
};
