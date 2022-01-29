Scriptname OSLAroused_MCM extends SKI_ConfigBase hidden

OSLAroused_Main Property Main Auto
OSLAroused_AdapterOStim Property OStimAdapter Auto

int Property CheckArousalKeyOid Auto
int Property EnableNudityCheckOid Auto
int Property EnableStatBuffsOid Auto

; OStim Specific Settings
int Property RequireLowArousalToEndSceneOid Auto

int function GetVersion()
    return 1
endfunction

Event OnConfigInit()
    ModName = "OSLAroused"
    Pages = new String[1]
    Pages[0] = "General Settings"
EndEvent

Event OnPageReset(string page)
    if(page == "" || page == "General Settings")
        CheckArousalKeyOid = AddKeyMapOption("Show Arousal Key", Main.GetShowArousalKeybind())
        EnableNudityCheckOid = AddToggleOption("Enable Nudity Increases Arousal", Main.GetEnableNudityIncreasesArousal())
        EnableStatBuffsOid = AddToggleOption("Enable Arousal Stat (De)Buffs", Main.EnableArousalStatBuffs)

        AddHeaderOption("OStim Settings")
        RequireLowArousalToEndSceneOid = AddToggleOption("Require Low Arousal To End Scene", OStimAdapter.RequireLowArousalToEndScene)
    endif
EndEvent

event OnOptionSelect(int optionId)
    if(CurrentPage == "General Settings" || CurrentPage == "")
        if (optionId == EnableNudityCheckOid)
            bool newVal = !Main.GetEnableNudityIncreasesArousal()
            Main.SetPlayerNudityIncreasesArousal(newVal) 
            SetToggleOptionValue(EnableNudityCheckOid, newVal)
        elseif (optionId == EnableStatBuffsOid)
            Main.EnableArousalStatBuffs = !Main.EnableArousalStatBuffs 
            SetToggleOptionValue(EnableNudityCheckOid, Main.EnableArousalStatBuffs)
        elseif (optionId == RequireLowArousalToEndSceneOid)
            OStimAdapter.RequireLowArousalToEndScene = !OStimAdapter.RequireLowArousalToEndScene 
            SetToggleOptionValue(RequireLowArousalToEndSceneOid, OStimAdapter.RequireLowArousalToEndScene)
        EndIf
    EndIf
endevent

Event OnOptionKeyMapChange(int optionId, int keyCode, string conflictControl, string conflictName)
    if(optionId == CheckArousalKeyOid)
        Main.SetShowArousalKeybind(keyCode)
        SetKeyMapOptionValue(CheckArousalKeyOid, keyCode)
    endif
EndEvent

event OnOptionHighlight(int optionId)
    if(CurrentPage == "General Settings" || CurrentPage == "")
        if(optionId == CheckArousalKeyOid)
            SetInfoText("Key To Show Arousal Bar")
        elseif(optionId == EnableNudityCheckOid)
            SetInfoText("If Enabled, Player Nudity will increase nearby NPC arrousal")
        elseif(optionId == EnableStatBuffsOid)
            SetInfoText("Will Enable Arousal based Stat Buffs")
        elseif(optionId == RequireLowArousalToEndSceneOid)
            SetInfoText("OStim Scene will not end until Participant arousal is low")
        EndIf
    EndIf
endevent