// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class TartarusEditorTarget : TargetRules
{
	public TartarusEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		bUseUnityBuild = false;
		bUsePCHFiles = false;

		ExtraModuleNames.Add("Tartarus");
	}
}
