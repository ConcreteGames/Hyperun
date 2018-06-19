// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AdvancedSteamSubsystemUtils : ModuleRules
{
	public AdvancedSteamSubsystemUtils(TargetInfo Target)
	{
        PrivateIncludePaths.Add("AdvancedSteamSubsystemUtils/Private");

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "OnlineSubsystem",
                "OnlineSubsystemUtils",
                "OnlineSubsystemSteam",
                "Steamworks",
                "AdvancedSteam",
                "Json",
            }
        );

        PrivateDependencyModuleNames.AddRange(
			new string[] { 
				"Core", 
				"CoreUObject",
				"Engine", 
                "Steamworks",
                "Json",
            }
		);

        PublicIncludePaths.AddRange(
            new string[] {
                "AdvancedSteamSubsystemUtils/Classes",
                "AdvancedSteam/Classes",
                "AdvancedSteam/Public",
            }
        );


        PrivateIncludePaths.AddRange(
            new string[] {
                "AdvancedSteamSubsystemUtils/Private",
                "OnlineSubsystemSteam/Private",
            }
        );

        AddEngineThirdPartyPrivateStaticDependencies(Target, "Steamworks");
    }
}
