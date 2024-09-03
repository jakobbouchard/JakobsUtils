using UnrealBuildTool;

public class JakobsUtilsDeveloper : ModuleRules
{
    public JakobsUtilsDeveloper(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "JakobsUtils",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "BlueprintGraph",
                "CoreUObject",
                "Engine",
                "GraphEditor",
                "Kismet",
                "KismetCompiler",
                "UnrealEd",
                "Slate",
                "SlateCore",
            }
        );
    }
}
