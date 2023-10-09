// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LyraGame : ModuleRules
{
	public LyraGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;


		//LyraWorldSettings里面#include "LyraLogChannels.h"就不报错了
		PublicIncludePaths.AddRange(
			new string[] {
				"LyraGame"
			}
		);
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"ApplicationCore",
				"Engine",
				
				
				"GameplayTags",
				"GameplayTasks",
				"GameplayAbilities",
				
				
				"ModularGameplay",
				"ModularGameplayActors",
				"GameFeatures",
				"CommonLoadingScreen",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"InputCore",
				"Slate",
				"SlateCore",
				"CommonGame",
				"CommonUser",
				"GameplayMessageRuntime",
				"EnhancedInput"
			}
		);
	}
}