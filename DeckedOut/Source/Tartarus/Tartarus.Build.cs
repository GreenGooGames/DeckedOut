// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Tartarus : ModuleRules
{
	public Tartarus(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        IWYUSupport = IWYUSupport.Full;

        PublicIncludePaths.AddRange(
			new string[] {
				"Tartarus"
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[] { 
				"Core", 
				"CoreUObject", 
				"Engine",
				"EnhancedInput",
				"InputCore", 
				"HeadMountedDisplay",
				"GameplayTags",
				"SlateCore",
				"CommonInput",
				"StateTreeModule",
				"GameplayStateTreeModule",
				"NavigationSystem",
				"AIModule",
				"Corrbolg",
				"Gleipnir"});
	}
}
