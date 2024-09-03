using UnrealBuildTool;

public class JakobsUtilsEditor : ModuleRules
{
    public JakobsUtilsEditor(ReadOnlyTargetRules Target) : base(Target)
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
                "EditorSubsystem",
                "UnrealEd",
            }
        );
    }
}
