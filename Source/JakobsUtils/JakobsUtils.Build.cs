using UnrealBuildTool;

public class JakobsUtils : ModuleRules
{
    public JakobsUtils(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "Steamworks",
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
