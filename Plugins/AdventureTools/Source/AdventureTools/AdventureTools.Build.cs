// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AdventureTools : ModuleRules
{
	public AdventureTools(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		// Modules that projects that use my plugin
		// will need to be able to see and use these deps
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Paper2D",
				"PaperZD",
				"Dialog",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		// Modules that are internal to the plugin and
		// don't need to be seen by projects using my plugin
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore", 
				"AdventureCommon", 
				"AIModule", 
				"EnhancedInput",
				"GameplayTags",
				"UMG"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
