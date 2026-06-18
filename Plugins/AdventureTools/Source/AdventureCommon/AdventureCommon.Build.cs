using UnrealBuildTool;

public class AdventureCommon : ModuleRules
{
    public AdventureCommon(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "GameplayTags"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Paper2D",
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore"
            }
        );
    }
}