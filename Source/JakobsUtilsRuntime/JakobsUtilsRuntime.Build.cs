using UnrealBuildTool;

public class JakobsUtilsRuntime : ModuleRules
{
    public JakobsUtilsRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
			    "EngineSettings",
			    "UMG",
            }
        );
    }
}
