namespace UnrealBuildTool.Rules
{
	public class LowEntryCompression : ModuleRules
	{
		public LowEntryCompression(TargetInfo Target)
		{
			PublicIncludePaths.AddRange(
				new string[] {
					"LowEntryCompression/Public/Module",
					"LowEntryCompression/Public/Classes",
					"LowEntryCompression/Public/Enums",
				}
			);

			PrivateIncludePaths.AddRange(
				new string[] {
					"LowEntryCompression/Private/Module",
					"LowEntryCompression/Private/Classes",
				}
			);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Engine",
					"Core",
					"CoreUObject",
				}
			);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
				}
			);

			DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
				}
			);
		}
	}
}
