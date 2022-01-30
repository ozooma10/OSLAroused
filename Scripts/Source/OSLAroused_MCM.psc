Scriptname OSLAroused_MCM extends SKI_ConfigBase hidden

OSLAroused_Main Property Main Auto
OSLAroused_AdapterOStim Property OStimAdapter Auto

int Property CheckArousalKeyOid Auto
int Property EnableNudityCheckOid Auto
int Property EnableStatBuffsOid Auto

string[] ArousalModeNames
int Property ArousalModeOid Auto

; OStim Specific Settings
int Property RequireLowArousalToEndSceneOid Auto

;---- Puppet Properties ----
Actor Property PuppetActor Auto
int Property SetArousalOid Auto
int Property SetMultiplierOid Auto
int Property SetTimeRateOid Auto


int function GetVersion()
    return 1
endfunction

Event OnConfigInit()
    ModName = "OSLAroused"
    Pages = new String[3]
    Pages[0] = "General Settings"
    Pages[1] = "Status"
    Pages[2] = "Puppeteer"

    ArousalModeNames = new string[2]
    ArousalModeNames[0] = "SexLab Aroused"
    ArousalModeNames[1] = "OAroused"

    PuppetActor = Game.GetPlayer()
EndEvent

Event OnPageReset(string page)
    SetCursorFillMode(TOP_TO_BOTTOM)
    if(page == "" || page == "General Settings")
        MainLeftColumn()
        SetCursorPosition(1)
        MainRightColumn()
    elseif(page == "Status")
        StatusPage()
    elseif(page == "Puppeteer")
        PuppeteerPage()
    endif
EndEvent

function MainLeftColumn()
    CheckArousalKeyOid = AddKeyMapOption("Show Arousal Key", Main.GetShowArousalKeybind())
    EnableStatBuffsOid = AddToggleOption("Enable Arousal Stat (De)Buffs", Main.EnableArousalStatBuffs)
    ArousalModeOid = AddMenuOption("Arousal Mode", ArousalModeNames[Main.GetCurrentArousalMode()])
endfunction

function MainRightColumn()
    AddHeaderOption("Nudity Settings")
    EnableNudityCheckOid = AddToggleOption("Player Nudity Increases Others Arousal", Main.GetEnableNudityIncreasesArousal())

    AddHeaderOption("OStim Settings")
    RequireLowArousalToEndSceneOid = AddToggleOption("Require Low Arousal To End Scene", OStimAdapter.RequireLowArousalToEndScene)
endfunction

function StatusPage()
    if(PuppetActor == none)
        AddHeaderOption("No Target Selected")
        return
    endif
    AddHeaderOption(PuppetActor.GetLeveledActorBase().GetName())

    int currentArousalMode = Main.GetCurrentArousalMode()
    if(currentArousalMode == Main.kArousalMode_OAroused)
        AddTextOption("Current Arousal", OSLArousedNative.GetArousal(PuppetActor), OPTION_FLAG_DISABLED)
        AddTextOption("Arousal Multiplier", OSLArousedNative.GetArousalMultiplier(PuppetActor), OPTION_FLAG_DISABLED)
    elseif(currentArousalMode == Main.kArousalMode_SLAroused)
        float timeRate = OSLArousedNative.GetTimeRate(PuppetActor)
        float lastOrgasm = OSLArousedNative.GetDaysSinceLastOrgasm(PuppetActor)

        AddTextOption("Arousal = Exposure + Time Arousal", OSLArousedNative.GetArousal(PuppetActor), OPTION_FLAG_DISABLED)
        AddTextOption("Current Exposure", OSLArousedNative.GetExposure(PuppetActor), OPTION_FLAG_DISABLED)
        AddTextOption("Exposure Rate", OSLArousedNative.GetArousalMultiplier(PuppetActor), OPTION_FLAG_DISABLED)
        AddTextOption("Time Arousal = D x (Time Rate)", lastOrgasm * timeRate, OPTION_FLAG_DISABLED)
        AddTextOption("D = Days Since Last Orgasm", OSLArousedNative.GetDaysSinceLastOrgasm(PuppetActor), OPTION_FLAG_DISABLED)
        AddTextOption("Time Rate", timeRate, OPTION_FLAG_DISABLED)
    endif
endfunction

function PuppeteerPage()
    if(PuppetActor == none)
        AddHeaderOption("No Target Selected")
        return
    endif

    AddEmptyOption()
    AddHeaderOption(PuppetActor.GetLeveledActorBase().GetName())

    int currentArousalMode = Main.GetCurrentArousalMode()
    if(currentArousalMode == Main.kArousalMode_OAroused)
        float exposure = OSLArousedNative.GetExposure(PuppetActor)
        SetArousalOid = AddSliderOption("Arousal", exposure, "{0}")
    
        float exposureRate = OSLArousedNative.GetArousalMultiplier(PuppetActor)
        SetMultiplierOid = AddSliderOption("Arousal Multiplier", exposureRate, "{0}")
    elseif(currentArousalMode == Main.kArousalMode_SLAroused)
        float exposure = OSLArousedNative.GetExposure(PuppetActor)
        SetArousalOid = AddSliderOption("Exposure", exposure, "{0}")
    
        float exposureRate = OSLArousedNative.GetArousalMultiplier(PuppetActor)
        SetMultiplierOid = AddSliderOption("Exposure Rate", exposureRate, "{1}")
        
        float timeRate = OSLArousedNative.GetTimeRate(PuppetActor)
        SetTimeRateOid = AddSliderOption("Time Rate", timeRate, "{0}")
    endif
endfunction

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
        elseif(optionId == ArousalModeOid)
            SetInfoText("SL Arousal emulates OG Sexlab Behavior. OArousal emulatues OArousal Behavior")
        EndIf
    EndIf
endevent

event OnOptionMenuOpen(int optionId)
    if(optionId == ArousalModeOid)
        SetMenuDialogStartIndex(Main.GetCurrentArousalMode())
        SetMenuDialogDefaultIndex(0)
        SetMenuDialogOptions(ArousalModeNames)
    endif
endevent

event OnOptionMenuAccept(int optionId, int index)
    if(optionId == ArousalModeOid)
        Main.SetCurrentArousalMode(index)
        SetMenuOptionValue(optionId, ArousalModeNames[index])
    endif
endevent

event OnOptionSliderOpen(int option)
    if(CurrentPage == "Puppeteer")
        if(option == SetArousalOid)
            float arousal = 0
            if(Main.GetCurrentArousalMode() == Main.kArousalMode_SLAroused)
                arousal = OSLArousedNative.GetExposure(PuppetActor)
            else
                arousal = OSLArousedNative.GetArousal(PuppetActor)
            endif
            SetSliderDialogStartValue(arousal)
            SetSliderDialogDefaultValue(0)
            SetSliderDialogRange(0, 100)
            SetSliderDialogInterval(1)
        elseif (option == SetMultiplierOid)
            float mult = OSLArousedNative.GetArousalMultiplier(PuppetActor)
            SetSliderDialogStartValue(mult)
            SetSliderDialogDefaultValue(2.0)
            SetSliderDialogRange(0, 10.0)
            SetSliderDialogInterval(0.1)
        elseif (option == SetTimeRateOid)
            float timeRate = OSLArousedNative.GetTimeRate(PuppetActor)
            SetSliderDialogStartValue(timeRate)
            SetSliderDialogDefaultValue(10.0)
            SetSliderDialogRange(0, 100.0)
            SetSliderDialogInterval(1.0)
        endif
    endif
endevent

event OnOptionSliderAccept(int option, float value)
    if(currentPage == "Puppeteer")
        if(option == SetArousalOid)
            OSLArousedNative.SetArousal(PuppetActor, value)
            SetSliderOptionValue(SetArousalOid, value, "{0}")
        elseif(option == SetMultiplierOid)
            OSLArousedNative.SetArousalMultiplier(PuppetActor, value)
            SetSliderOptionValue(SetMultiplierOid, value, "{1}")
        elseif(option == SetTimeRateOid)
            OSLArousedNative.SetTimeRate(PuppetActor, value)
            SetSliderOptionValue(SetTimeRateOid, value, "{0}")
        endif
    endif
endevent

event OnOptionDefault(int option)
    if(currentPage == "Puppeteer")
        if(option == SetArousalOid)
            OSLArousedNative.SetArousal(PuppetActor, 0)
            SetSliderOptionValue(SetArousalOid, 0, "{0}")
        elseif(option == SetMultiplierOid)
            OSLArousedNative.SetArousalMultiplier(PuppetActor, 2.0)
            SetSliderOptionValue(SetMultiplierOid, 2.0, "{1}")
        elseif(option == SetTimeRateOid)
            OSLArousedNative.SetTimeRate(PuppetActor, 10.0)
            SetSliderOptionValue(SetTimeRateOid, 10.0, "{0}")
        endif
    endif
endevent

event OnKeyDown(int keyCode)
    if(!Utility.IsInMenuMode() && keyCode == Main.GetShowArousalKeybind())
        Actor target = Game.GetCurrentCrosshairRef() as Actor
        if(target != none)
            PuppetActor = target
        Else
            PuppetActor = Game.GetPlayer()
        endif
    endif
endevent