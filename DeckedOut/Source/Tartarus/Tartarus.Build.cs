// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Tartarus : ModuleRules
{
	public Tartarus(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

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
				"CommonInput"});
	}
}
