using UnrealBuildTool;
using System.Collections.Generic;

public class FootballArenaEditorTarget : TargetRules
{
    public FootballArenaEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        ExtraModuleNames.Add("FootballArena");
    }
}
