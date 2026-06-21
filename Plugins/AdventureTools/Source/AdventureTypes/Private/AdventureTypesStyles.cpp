#include "AdventureTypesStyles.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateStyleMacros.h"

FAdventureTypesStyles& FAdventureTypesStyles::Get()
{
	static FAdventureTypesStyles Instance;
	return Instance;
}

void FAdventureTypesStyles::SetUp()
{
	/// Class Icon is 16 x 16, usually PNG, used in content browser, actions menu
	const FVector2D Icon16x16(16.0f, 16.0f);
		
	/// These are mostly used for editor button, eg: FooEditor.Flip.Small
	const FVector2D Icon20x20(20.0f, 20.0f);
		
	/// These are mostly used for editor button, eg: FooEditor.Flip
	const FVector2D Icon40x40(40.0f, 40.0f);
		
	/// Class Thumbnail is 64 x 46, can be SVG, used in scene
	const FVector2D Icon64x64(64.0f, 64.0f);

	/// Note that the plugin could be moved to the engine, or to the project at various times.
	const FString PluginDir = IPluginManager::Get().FindPlugin(TEXT("AdventureTools"))->GetBaseDir();
	SetContentRoot(PluginDir / TEXT("Content"));
		
	/// Code inside Source/Editor/UnrealEd/Private/AssetThumbnail.cpp searches for these special strings:
	/// "ClassThumbnail" + <class name> and automatically assigns them as class icons and thumbnails
	/// as long as they're registered.
	Set("ClassThumbnail.ItemTypeDefs", new IMAGE_BRUSH_SVG("Slate/AdventureItemTypeDefs", Icon64x64, FStyleColors::AccentOrange));
	Set("ClassIcon.ItemTypeDefs", new IMAGE_BRUSH("Slate/AdventureItemTypeDefs_16x", Icon16x16, FStyleColors::AccentOrange));
	
	Set("ClassThumbnail.StoryAction", new IMAGE_BRUSH_SVG("Slate/StoryAction", Icon64x64, FStyleColors::AccentOrange));
	Set("ClassIcon.StoryAction", new IMAGE_BRUSH("Slate/StoryAction_16x", Icon16x16, FStyleColors::AccentOrange));
	
	Set("ClassThumbnail.Item", new IMAGE_BRUSH_SVG("Slate/Item", Icon64x64, FStyleColors::AccentOrange));
	Set("ClassIcon.Item", new IMAGE_BRUSH("Slate/Item_16x", Icon16x16, FStyleColors::AccentOrange));
	
	FSlateStyleRegistry::RegisterSlateStyle(*this);
}

void FAdventureTypesStyles::TearDown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*this);
}
