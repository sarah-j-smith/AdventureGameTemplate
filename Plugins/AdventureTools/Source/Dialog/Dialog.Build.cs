using UnrealBuildTool;

public class Dialog : ModuleRules
{
    public Dialog(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "Engine", "AdventureCommon", 
                "CoreUObject",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Slate",
                "SlateCore",
                "AdventureCommon",
                "UMG"
            }
        );
    }
    
}