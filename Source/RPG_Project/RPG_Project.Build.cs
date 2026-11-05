// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RPG_Project : ModuleRules
{
	public RPG_Project(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		// windows.h 与虚幻引擎 GAS 插件中的 max 函数宏发生冲突，一般所有的GAS项目都需要加上PublicDefinitions.Add("NOMINMAX");
		PublicDefinitions.Add("NOMINMAX");
		PublicDependencyModuleNames.AddRange(new string[] { "Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayTags",
			"GameplayTasks",
			"AnimGraphRuntime",
			"MotionWarping",
			"Niagara",
			"GameplayAbilities",
			"NavigationSystem",
			"MoviePlayer"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "MoviePlayer", "MoviePlayer", "MoviePlayer" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
