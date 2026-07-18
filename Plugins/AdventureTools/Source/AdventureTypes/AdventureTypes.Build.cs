using System.IO;
using UnrealBuildTool;

public class AdventureTypes : ModuleRules
{
    public AdventureTypes(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicIncludePaths.AddRange(
            new string[]
            {
                Path.Combine(ModuleDirectory, "Public")
            });

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "AdventureCommon",
                "UnrealEd",
                "Engine",
                "AssetDefinition",
                "AssetRegistry"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "PlacementMode",
                
                //Check which are needed
                "PropertyEditor",
                "LevelEditor",
                "UnrealEd",
                "Projects", 
                "AdventureTools"
            }
        );
    }
}