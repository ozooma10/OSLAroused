Scriptname OSLAroused_MCM extends SKI_ConfigBase hidden

OSLAroused_Main Property Main Auto

OSLAroused_MCM Function Get() Global
	return Game.GetFormFromFile(0x806, "OSLAroused.esp") as OSLAroused_MCM
EndFunction

int CheckArousalKeyOid
int EnableStatBuffsOid
int EnableNudityCheckOid
int HourlyNudityArousalModOid
int HourlySceneParticipantArousalModOid
int HourlySceneViewerArousalModOid

int EnableDebugModeOid

int VictimGainsArousalOid

; OStim Specific Settings
int Property RequireLowArousalToEndSceneOid Auto

; Sexlab Specfic Settings
int StageChangeIncreasesArousalOid
int SexlabStageChangeArousalGainOid

;---- Puppet Properties ----
Actor Property PuppetActor Auto
int Property SetArousalOid Auto
int Property SetMultiplierOid Auto
int Property SetTimeRateOid Auto

float Property kDefaultArousalMultiplier = 1.0 AutoReadOnly

;------ System Properties -------
int DumpArousalData
int ClearSecondaryArousalData
int ClearAllArousalData

int SLAStubLoadedOid
int OArousedStubLoadedOid

;------ Keywords -------
int ArmorListMenuOid
Armor SelectedArmor
string[] FoundArmorNames
int[] FoundArmorIds

int EroticArmorOid
Keyword Property EroticArmorKeyword Auto
bool EroticArmorState

int BikiniArmorOid
Keyword BikiniArmorKeyword
bool BikiniArmorState

int SLAArmorPrettyOid
Keyword SLAArmorPrettyKeyword
bool SLAArmorPrettyState

int SLAArmorHalfNakedOid
Keyword SLAArmorHalfNakedKeyword
bool SLAArmorHalfNakedState

int SLAArmorSpendexOid
Keyword SLAArmorSpendexKeyword
bool SLAArmorSpendexState

int SLAHasStockingsOid
Keyword SLAHasStockingsKeyword
bool SLAHasStockingsState

int function GetVersion()
    return 2 ; 0.1.1
endfunction

Event OnGameLoaded()
    ModName = "OSL Aroused"
    Pages = new String[5]
    Pages[0] = "General Settings"
    Pages[1] = "Status"
    Pages[2] = "Puppeteer"
    Pages[3] = "Keywords"
    Pages[4] = "System"

    PuppetActor = Game.GetPlayer()
    
	EroticArmorKeyword = Keyword.GetKeyword("EroticArmor")
	BikiniArmorKeyword = Keyword.GetKeyword("_SLS_BikiniArmor")
	SLAArmorPrettyKeyword = Keyword.GetKeyword("SLA_ArmorPretty")
	SLAArmorHalfNakedKeyword = Keyword.GetKeyword("SLA_ArmorHalfNaked")
	SLAArmorSpendexKeyword = Keyword.GetKeyword("SLA_ArmorSpendex")
	SLAHasStockingsKeyword = Keyword.GetKeyword("SLA_HasStockings")
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
    elseif(page == "Keywords")
        KeywordPage()
    elseif(page == "System")
        SystemPage()
    endif
EndEvent

function MainLeftColumn()
    CheckArousalKeyOid = AddKeyMapOption("Show Arousal Key", Main.GetShowArousalKeybind())
    EnableStatBuffsOid = AddToggleOption("Enable Arousal Stat (De)Buffs", Main.EnableArousalStatBuffs)

    AddHeaderOption("Scene Settings")
    HourlySceneParticipantArousalModOid = AddSliderOption("Hourly Arousal From Participating", Main.GetHourlySceneParticipantArousalModifier(), "{1}")
    HourlySceneViewerArousalModOid = AddSliderOption("Hourly Arousal From Spectating", Main.GetHourlySceneViewerArousalModifier(), "{1}")
    VictimGainsArousalOid = AddToggleOption("Victim Gains Arousal", Main.VictimGainsArousal)

endfunction

function MainRightColumn()
    AddHeaderOption("Nudity Settings")
    EnableNudityCheckOid = AddToggleOption("Player Nudity Increases Others Arousal", Main.GetEnableNudityIncreasesArousal())
    HourlyNudityArousalModOid = AddSliderOption("Hourly Arousal From Viewing Nude", Main.GetHourlyNudityArousalModifier(), "{1}")

    AddHeaderOption("OStim Settings")
    RequireLowArousalToEndSceneOid = AddToggleOption("Require Low Arousal To End Scene", Main.RequireLowArousalToEndScene)

    AddHeaderOption("Sexlab Settings")
    StageChangeIncreasesArousalOid = AddToggleOption("Stage change Increases Arousal", Main.SexlabStageChangeIncreasesArousal)
    SexlabStageChangeArousalGainOid = AddSliderOption("Stage Change Arousal Gain", Main.SexlabStageChangeArousalGain, "{1}")
endfunction

function StatusPage()
    if(PuppetActor == none)
        AddHeaderOption("No Target Selected")
        return
    endif
    AddHeaderOption(PuppetActor.GetLeveledActorBase().GetName())

    float timeRate = OSLArousedNative.GetTimeRate(PuppetActor)
    float lastOrgasm = OSLArousedNative.GetDaysSinceLastOrgasm(PuppetActor)

    AddTextOption("Arousal = Exposure + Time Arousal", OSLArousedNative.GetArousal(PuppetActor), OPTION_FLAG_DISABLED)
    AddTextOption("Current Exposure", OSLArousedNative.GetExposure(PuppetActor), OPTION_FLAG_DISABLED)
    AddTextOption("Exposure Rate", OSLArousedNative.GetArousalMultiplier(PuppetActor), OPTION_FLAG_DISABLED)
    AddTextOption("Time Arousal = D x (Time Rate)", lastOrgasm * timeRate, OPTION_FLAG_DISABLED)
    AddTextOption("D = Days Since Last Orgasm", OSLArousedNative.GetDaysSinceLastOrgasm(PuppetActor), OPTION_FLAG_DISABLED)
    AddTextOption("Time Rate", timeRate, OPTION_FLAG_DISABLED)
endfunction

function PuppeteerPage()
    if(PuppetActor == none)
        AddHeaderOption("No Target Selected")
        return
    endif
    AddHeaderOption(PuppetActor.GetLeveledActorBase().GetName())

    float exposure = OSLArousedNative.GetExposure(PuppetActor)
    SetArousalOid = AddSliderOption("Exposure", exposure, "{0}")

    float exposureRate = OSLArousedNative.GetArousalMultiplier(PuppetActor)
    SetMultiplierOid = AddSliderOption("Exposure Rate", exposureRate, "{1}")
    
    float timeRate = OSLArousedNative.GetTimeRate(PuppetActor)
    SetTimeRateOid = AddSliderOption("Time Rate", timeRate, "{0}")
endfunction

function KeywordPage()
    AddHeaderOption("Keyword Management")
    ArmorListMenuOid = AddMenuOption("Click to Load Armor List", "")
    SetCursorPosition(1)
    EroticArmorOid = AddToggleOption("EroticArmor", false, OPTION_FLAG_DISABLED)
    BikiniArmorOid = AddToggleOption("SLS Bikini Armor", false, OPTION_FLAG_DISABLED)
    SLAArmorPrettyOid = AddToggleOption("SLA_ArmorPretty", false, OPTION_FLAG_DISABLED)
    SLAArmorHalfNakedOid = AddToggleOption("SLA_ArmorHalfNaked", false, OPTION_FLAG_DISABLED)
    SLAArmorSpendexOid = AddToggleOption("SLA_ArmorSpendex", false, OPTION_FLAG_DISABLED)
    SLAHasStockingsOid = AddToggleOption("SLA_HasStockings", false, OPTION_FLAG_DISABLED)
endfunction

function SystemPage()
    AddHeaderOption("Native Data")
    DumpArousalData = AddTextOption("Dump Arousal Data", "RUN")
    ;ClearSecondaryArousalData = AddTextOption("Clear Secondary Arousal Data", "RUN")
    ClearAllArousalData = AddTextOption("Clear All Arousal Data", "RUN")
    EnableDebugModeOid = AddToggleOption("Enable Debug Logging", Main.EnableDebugMode)

    SetCursorPosition(1)
    AddHeaderOption("Framework Adapters")
    If (Main.SexLabAdapterLoaded)
        AddTextOption("SexLab", "Enabled")
    Else
        AddTextOption("SexLab", "Disabled", OPTION_FLAG_DISABLED)
    EndIf
    If (Main.OStimAdapterLoaded)
        AddTextOption("OStim", "Enabled")
    Else
        AddTextOption("OStim", "Disabled", OPTION_FLAG_DISABLED)
    EndIf
    AddHeaderOption("Arousal Compatability")
    If (Main.InvalidSlaFound)
        SLAStubLoadedOid = AddTextOption("SexLab Aroused", "Invalid Install")
    ElseIf (Main.SlaStubLoaded)
        AddTextOption("SexLab Aroused", "Enabled")
    Else
        SLAStubLoadedOid = AddTextOption("SexLab Aroused", "Disabled")
    EndIf
    If (Main.InvalidOArousedFound)
        OArousedStubLoadedOid = AddTextOption("OAroused", "Invalid Install")
    elseIf (Main.OArousedStubLoaded)
        AddTextOption("OAroused", "Enabled")
    Else
        OArousedStubLoadedOid = AddTextOption("OAroused", "Disabled")
    EndIf
endfunction

event OnOptionSelect(int optionId)
    if(CurrentPage == "General Settings" || CurrentPage == "")
        if (optionId == EnableNudityCheckOid)
            bool newVal = !Main.GetEnableNudityIncreasesArousal()
            Main.SetPlayerNudityIncreasesArousal(newVal) 
            SetToggleOptionValue(EnableNudityCheckOid, newVal)
        elseif (optionId == EnableStatBuffsOid)
            Main.SetArousalEffectsEnabled(!Main.EnableArousalStatBuffs) 
            SetToggleOptionValue(EnableStatBuffsOid, Main.EnableArousalStatBuffs)
        elseif (optionId == RequireLowArousalToEndSceneOid)
            Main.RequireLowArousalToEndScene = !Main.RequireLowArousalToEndScene 
            SetToggleOptionValue(RequireLowArousalToEndSceneOid, Main.RequireLowArousalToEndScene)
        elseif (optionId == StageChangeIncreasesArousalOid)
            Main.SexlabStageChangeIncreasesArousal = !Main.SexlabStageChangeIncreasesArousal
            SetToggleOptionValue(StageChangeIncreasesArousalOid, Main.SexlabStageChangeIncreasesArousal)
        elseif (optionId == VictimGainsArousalOid)
            Main.VictimGainsArousal = !Main.VictimGainsArousal
            SetToggleOptionValue(VictimGainsArousalOid, Main.VictimGainsArousal)
        EndIf
    ElseIf (CurrentPage == "Keywords")
        if(optionId == EroticArmorOid)
            if(EroticArmorState)
                bool removeSuccess = OSLArousedNative.RemoveKeywordFromForm(SelectedArmor, EroticArmorKeyword)
                EroticArmorState = !removeSuccess ;if remove success fails, indicate keyword still on
            else
                bool updateSuccess = OSLArousedNative.AddKeywordToForm(SelectedArmor, EroticArmorKeyword)
                EroticArmorState = updateSuccess
            endif
            SetToggleOptionValue(EroticArmorOid, EroticArmorState)
        elseif(optionId == BikiniArmorOid)
            if(BikiniArmorState)
                bool removeSuccess = OSLArousedNative.RemoveKeywordFromForm(SelectedArmor, BikiniArmorKeyword)
                BikiniArmorState = !removeSuccess ;if remove success fails, indicate keyword still on
            else
                bool updateSuccess = OSLArousedNative.AddKeywordToForm(SelectedArmor, BikiniArmorKeyword)
                BikiniArmorState = updateSuccess
            endif
            SetToggleOptionValue(BikiniArmorOid, BikiniArmorState)
        elseif(optionId == SLAArmorPrettyOid)
            if(SLAArmorPrettyState)
                bool removeSuccess = OSLArousedNative.RemoveKeywordFromForm(SelectedArmor, SLAArmorPrettyKeyword)
                SLAArmorPrettyState = !removeSuccess ;if remove success fails, indicate keyword still on
            else
                bool updateSuccess = OSLArousedNative.AddKeywordToForm(SelectedArmor, SLAArmorPrettyKeyword)
                SLAArmorPrettyState = updateSuccess
            endif
            SetToggleOptionValue(SLAArmorPrettyOid, SLAArmorPrettyState)
        elseif(optionId == SLAArmorHalfNakedOid)
            if(SLAArmorHalfNakedState)
                bool removeSuccess = OSLArousedNative.RemoveKeywordFromForm(SelectedArmor, SLAArmorHalfNakedKeyword)
                SLAArmorHalfNakedState = !removeSuccess ;if remove success fails, indicate keyword still on
            else
                bool updateSuccess = OSLArousedNative.AddKeywordToForm(SelectedArmor, SLAArmorHalfNakedKeyword)
                SLAArmorHalfNakedState = updateSuccess
            endif
            SetToggleOptionValue(SLAArmorHalfNakedOid, SLAArmorHalfNakedState)
        elseif(optionId == SLAArmorSpendexOid)
            if(SLAArmorSpendexState)
                bool removeSuccess = OSLArousedNative.RemoveKeywordFromForm(SelectedArmor, SLAArmorSpendexKeyword)
                SLAArmorSpendexState = !removeSuccess ;if remove success fails, indicate keyword still on
            else
                bool updateSuccess = OSLArousedNative.AddKeywordToForm(SelectedArmor, SLAArmorSpendexKeyword)
                SLAArmorSpendexState = updateSuccess
            endif
            SetToggleOptionValue(SLAArmorSpendexOid, SLAArmorSpendexState)
        elseif(optionId == SLAHasStockingsOid)
            if(SLAHasStockingsState)
                bool removeSuccess = OSLArousedNative.RemoveKeywordFromForm(SelectedArmor, SLAHasStockingsKeyword)
                SLAHasStockingsState = !removeSuccess ;if remove success fails, indicate keyword still on
            else
                bool updateSuccess = OSLArousedNative.AddKeywordToForm(SelectedArmor, SLAHasStockingsKeyword)
                SLAHasStockingsState = updateSuccess
            endif
            SetToggleOptionValue(SLAHasStockingsOid, SLAHasStockingsState)
        endif
    ElseIf(CurrentPage == "System")
        if(optionId == DumpArousalData)
            OSLArousedNative.DumpArousalData()
        elseif(optionId == ClearSecondaryArousalData)
            if (ShowMessage("Are you sure you want to Clear Secondary NPC Arousal Data? This is non-reversible"))
                OSLArousedNative.ClearSecondaryArousalData()
            endif
        elseif(optionId == ClearAllArousalData)
            if (ShowMessage("Are you sure you want to Clear All Arousal Data? This is non-reversible"))
                OSLArousedNative.ClearAllArousalData()
            endif
        ElseIf (optionId == EnableDebugModeOid)
            Main.EnableDebugMode = !Main.EnableDebugMode
            SetToggleOptionValue(EnableDebugModeOid, Main.EnableDebugMode)
        endif
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
        elseif(optionId == HourlyNudityArousalModOid)
            SetInfoText("Arousal Gained per hour when observing a nude character.")
        elseif(optionId == HourlySceneParticipantArousalModOid)
            SetInfoText("Arousal Gained per hour when participating in a sex scene.")
        elseif(optionId == HourlySceneViewerArousalModOid)
            SetInfoText("Arousal Gained per hour for spectators of a sex scene.")
        elseif(optionId == EnableStatBuffsOid)
            SetInfoText("Will Enable Arousal based Stat Buffs")
        elseif(optionId == RequireLowArousalToEndSceneOid)
            SetInfoText("OStim Scene will not end until Participant arousal is low")
        elseif(optionId == StageChangeIncreasesArousalOid)
            SetInfoText("Changing Scene stage increases participant arousal")
        elseif(optionId == SexlabStageChangeArousalGainOid)
            SetInfoText("Arousal gained when sexlab stage changes")
        elseif(optionId == VictimGainsArousalOid)
            SetInfoText("Victim gains arousal in scenes")
        EndIf
    elseif(CurrentPage == "System")
        if(optionId == DumpArousalData)
            SetInfoText("Dump all stored arousal data to SKSE log file")
        elseif(optionId == ClearSecondaryArousalData)
            SetInfoText("Clear NPC Arousal data from Save (This Maintains Player/Unique Data)")
        elseif(optionId == ClearAllArousalData)
            SetInfoText("Clear All Arousal data from Save")
        elseif(optionId == SLAStubLoadedOid)
            If (Main.InvalidSlaFound)
                SetInfoText("Incorrect SexlabAroused.esm or slaFrameworkScr.pex detected. Ensure SLA is not installed and OSL Aroused overwrites all conflicts.")
            elseif(!Main.SlaStubLoaded)
                SetInfoText("SexlabAroused.esm is disabled or missing. SLA backwards compatibility is disabled.")
            EndIf
        elseif(optionId == OArousedStubLoadedOid)
            If (Main.InvalidOArousedFound)
                SetInfoText("Incorrect OAroused.esp or OArousedScript.pex detected. Ensure OAroused is not installed and OSL Aroused overwrites all conflicts.")
            elseif(!Main.OArousedStubLoaded)
                SetInfoText("OAroused.esp is disabled or missing. OAroused backwards compatibility is disabled.")
            EndIf
        endif
    EndIf
endevent

event OnOptionMenuOpen(int optionId)
    if(CurrentPage == "General Settings" || CurrentPage == "")
    elseif (CurrentPage == "Keywords")
        if(optionId == ArmorListMenuOid)
            LoadArmorList()
        endif
    endif
endevent

event OnOptionMenuAccept(int optionId, int index)
    if(CurrentPage == "General Settings" || CurrentPage == "")
    ElseIf (CurrentPage == "Keywords")
        If (optionId == ArmorListMenuOid)
            SelectedArmor = Game.GetPlayer().GetNthForm(FoundArmorIds[index]) as Armor
            SetMenuOptionValue(optionId, FoundArmorNames[index])
            ArmorSelected()
        EndIf
    endif
endevent

event OnOptionSliderOpen(int option)
    if(CurrentPage == "" || CurrentPage == "General Settings")
        if(option == HourlyNudityArousalModOid)
            SetSliderDialogStartValue(Main.GetHourlyNudityArousalModifier())
            SetSliderDialogDefaultValue(20.0)
            SetSliderDialogRange(0, 100)
            SetSliderDialogInterval(0.1)
        elseif(option == HourlySceneParticipantArousalModOid)
            SetSliderDialogStartValue(Main.GetHourlySceneParticipantArousalModifier())
            SetSliderDialogDefaultValue(20.0)
            SetSliderDialogRange(0, 100)
            SetSliderDialogInterval(0.1)
        elseif(option == HourlySceneViewerArousalModOid)
            SetSliderDialogStartValue(Main.GetHourlySceneViewerArousalModifier())
            SetSliderDialogDefaultValue(20.0)
            SetSliderDialogRange(0, 100)
            SetSliderDialogInterval(0.1)
        elseif(option == SexlabStageChangeArousalGainOid)
            SetSliderDialogStartValue(Main.SexlabStageChangeArousalGain)
            SetSliderDialogDefaultValue(3.0)
            SetSliderDialogRange(0, 10)
            SetSliderDialogInterval(0.1)
        endif
    elseif(CurrentPage == "Puppeteer")
        if(option == SetArousalOid)
            float arousal = 0
            arousal = OSLArousedNative.GetExposure(PuppetActor)
            SetSliderDialogStartValue(arousal)
            SetSliderDialogDefaultValue(0)
            SetSliderDialogRange(0, 100)
            SetSliderDialogInterval(1)
        elseif (option == SetMultiplierOid)
            float mult = OSLArousedNative.GetArousalMultiplier(PuppetActor)
            SetSliderDialogStartValue(mult)
            SetSliderDialogDefaultValue(kDefaultArousalMultiplier)
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
    if(CurrentPage == "" || CurrentPage == "General Settings")
        if(option == HourlyNudityArousalModOid)
            Main.SetHourlyNudityArousalModifier(value)
            SetSliderOptionValue(HourlyNudityArousalModOid, value, "{1}")
        elseif(option == HourlySceneParticipantArousalModOid)
            Main.SetHourlySceneParticipantArousalModifier(value)
            SetSliderOptionValue(HourlySceneParticipantArousalModOid, value, "{1}")
        elseif(option == HourlySceneViewerArousalModOid)
            Main.SetHourlySceneViewerArousalModifier(value)
            SetSliderOptionValue(HourlySceneViewerArousalModOid, value, "{1}")
        elseif(option == SexlabStageChangeArousalGainOid)
            Main.SexlabStageChangeArousalGain = value
            SetSliderOptionValue(SexlabStageChangeArousalGainOid, value, "{1}")
        endif
    elseif(currentPage == "Puppeteer")
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
    if(CurrentPage == "" || CurrentPage == "General Settings")
        if(option == HourlyNudityArousalModOid)
            Main.SetHourlyNudityArousalModifier(20.0)
            SetSliderOptionValue(HourlyNudityArousalModOid, 20.0, "{1}")
        elseif(option == HourlySceneParticipantArousalModOid)
            Main.SetHourlySceneParticipantArousalModifier(20.0)
            SetSliderOptionValue(HourlySceneParticipantArousalModOid, 20.0, "{1}")
        elseif(option == HourlySceneViewerArousalModOid)
            Main.SetHourlySceneViewerArousalModifier(20.0)
            SetSliderOptionValue(HourlySceneViewerArousalModOid, 20.0, "{1}")
        elseif(option == SexlabStageChangeArousalGainOid)
            Main.SexlabStageChangeArousalGain = 3.0
            SetSliderOptionValue(SexlabStageChangeArousalGainOid, 3.0, "{1}")
        endif
    elseif(currentPage == "Puppeteer")
        if(option == SetArousalOid)
            OSLArousedNative.SetArousal(PuppetActor, 0)
            SetSliderOptionValue(SetArousalOid, 0, "{0}")
        elseif(option == SetMultiplierOid)
            OSLArousedNative.SetArousalMultiplier(PuppetActor, kDefaultArousalMultiplier)
            SetSliderOptionValue(SetMultiplierOid, kDefaultArousalMultiplier, "{1}")
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

;Based off code from MrowrPurr :)
function LoadArmorList()
    SelectedArmor = none
    Actor player = Game.GetPlayer()
    int numItems = player.GetNumItems()
    int index = 0
    FoundArmorNames = new string[128]
    FoundArmorIds = new int[128]
    int foundItemIndex = 0
    while(index < numItems && foundItemIndex < 128)
        Armor armorItem = player.GetNthForm(index) as Armor
        if(armorItem)
            FoundArmorNames[foundItemIndex] = armorItem.GetName()
            FoundArmorIds[foundItemIndex] = index
            foundItemIndex += 1
        endif
        index += 1
    endwhile

    FoundArmorNames = Utility.ResizeStringArray(FoundArmorNames, foundItemIndex)
    FoundArmorIds = Utility.ResizeIntArray(FoundArmorIds, foundItemIndex)
    SetMenuDialogOptions(FoundArmorNames)
endfunction

function ArmorSelected()
    if(!SelectedArmor)
        return
    endif

    EroticArmorState = CheckKeyword(EroticArmorKeyword, EroticArmorOid)
    BikiniArmorState = CheckKeyword(BikiniArmorKeyword, BikiniArmorOid)
    SLAArmorPrettyState = CheckKeyword(SLAArmorPrettyKeyword, SLAArmorPrettyOid)
    SLAArmorHalfNakedState = CheckKeyword(SLAArmorHalfNakedKeyword, SLAArmorHalfNakedOid)
    SLAArmorSpendexState = CheckKeyword(SLAArmorSpendexKeyword, SLAArmorSpendexOid)
    SLAHasStockingsState = CheckKeyword(SLAHasStockingsKeyword, SLAHasStockingsOid)
endfunction

bool function CheckKeyword(Keyword armorKeyword, int oid)
    bool keywordEnabled
    if(armorKeyword)
        SetOptionFlags(oid, OPTION_FLAG_NONE)
        keywordEnabled = SelectedArmor.HasKeyword(armorKeyword)
        SetToggleOptionValue(oid, keywordEnabled)
    else
        SetToggleOptionValue(oid, false)
    endif

    return keywordEnabled
endfunction

function Log(string msg) global
    Debug.Trace("---OSLAroused--- [MCM] " + msg)
endfunction
