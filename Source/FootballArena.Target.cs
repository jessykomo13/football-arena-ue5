using UnrealBuildTool;
using System.Collections.Generic;

public class FootballArenaTarget : TargetRules
{
    public FootballArenaTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        ExtraModuleNames.Add("FootballArena");
    }
}
