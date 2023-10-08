// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class LyraServerTarget : TargetRules
{
	public LyraServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;

		ExtraModuleNames.Add("LyraGame");

		//在项目的“Shipping”构建配置中启用额外的检查。这些检查通常用于确保在最终发布版本中没有包含不必要的调试信息、资产或代码。
		//启用这些检查可以帮助确保最终发布的游戏版本是高质量且稳定的
		bUseChecksInShipping = true;
	}
}
