// Copyright Epic Games, Inc. All Rights Reserved.

#include "AdventureTools.h"
#include "Constants.h"

#include "Internationalization/StringTableRegistry.h"

#define LOCTEXT_NAMESPACE "FAdventureToolsModule"

DEFINE_LOG_CATEGORY(LogAdventureGame);

// #define DEBUG_STRING_TABLES 1

void FAdventureToolsModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FStringTableRegistry::Get().UnregisterStringTable(ITEM_STRINGS_KEY);
	FStringTableRegistry::Get().UnregisterStringTable(ITEM_DESCRIPTIONS_KEY);
	FStringTableRegistry::Get().UnregisterStringTable(ITEM_LONG_DESCRIPTIONS_KEY);
	FStringTableRegistry::Get().UnregisterStringTable(UI_STRINGS_KEY);

	LOCTABLE_FROMFILE_GAME(ITEM_STRINGS_KEY, ITEMS_NAMESPACE_KEY, "StringTables/ItemStrings.csv");
	LOCTABLE_FROMFILE_GAME(ITEM_DESCRIPTIONS_KEY, ITEMS_NAMESPACE_KEY, "StringTables/ItemDescriptions.csv");
	LOCTABLE_FROMFILE_GAME(ITEM_LONG_DESCRIPTIONS_KEY, ITEMS_NAMESPACE_KEY, "StringTables/ItemLongDescriptions.csv");
	LOCTABLE_FROMFILE_GAME(UI_STRINGS_KEY, UI_NAMESPACE_KEY, "StringTables/UIStrings.csv");

#ifdef DEBUG_STRING_TABLES
	FStringTableRegistry::Get().EnumerateStringTables([](const FName &StringtableName, const FStringTableConstRef &Ref)
	{
		UE_LOG(LogAdventureGame, Log, TEXT("String table %s"), *StringtableName.ToString());
		Ref.Get().EnumerateSourceStrings([](const FString &Key, const FString &Value)
		{
			UE_LOG(LogAdventureGame, Warning, TEXT("    %s - %s"), *Key, *Value);
			return true;
		});
		return true;
	});
#endif
}

void FAdventureToolsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAdventureToolsModule, AdventureTools)