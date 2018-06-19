using UnrealBuildTool;
using System.IO;
using System;

public class AdvancedSteam : ModuleRules
{
    public AdvancedSteam(TargetInfo Target)
    {
        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "OnlineSubsystem",
                "OnlineSubsystemUtils",
                "OnlineSubsystemSteam", 
                "Steamworks",
                "Networking",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "OnlineSubsystemSteam",
                "Steamworks",
            }
        );

        PublicIncludePaths.AddRange(
            new string[] {
                "AdvancedSteam/Public",
                "AdvancedSteam/Classes",
            }
        );


        PrivateIncludePaths.AddRange(
            new string[] {
                "AdvancedSteam/Private",
			}
        );

        AddEngineThirdPartyPrivateStaticDependencies(Target, "Steamworks");        
    }
}
