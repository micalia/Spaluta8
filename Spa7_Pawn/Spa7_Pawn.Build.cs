// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Spa7_Pawn : ModuleRules
{
	public Spa7_Pawn(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG" });
	}
}
