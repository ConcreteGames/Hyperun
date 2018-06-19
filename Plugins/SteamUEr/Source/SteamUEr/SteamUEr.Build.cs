// Some copyright should be here...

using UnrealBuildTool;

public class SteamUEr : ModuleRules
{
	public SteamUEr(TargetInfo Target)
	{
		
		PublicIncludePaths.AddRange(
			new string[] {
				"SteamUEr/Public"
				
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"SteamUEr/Private",
				
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
				
				// ... add other public dependencies that you statically link with here ...
			}
            );
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "Steamworks"
				// ... add private dependencies that you statically link with here ...	
			}
			);
        
       
	}
}
