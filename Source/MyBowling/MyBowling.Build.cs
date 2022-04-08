// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MyBowling : ModuleRules
{
	public MyBowling(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
