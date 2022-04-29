Scriptname OSLAroused_MCM extends SKI_ConfigBase hidden

OSLAroused_Main Property Main Auto

OSLAroused_MCM Function Get() Global
	return Game.GetFormFromFile(0x806, "OSLAroused.esp") as OSLAroused_MCM
EndFunction

;---- Overview Properties ----
int ArousalStatusOid
int BaselineArousalStatusOid
int LibidoStatusOid
int ArousalMultiplierStatusOid

int SLAStubLoadedOid
int OArousedStubLoadedOid

;---- Settings ----
;Baseline
int BeingNudeBaselineOid
int ViewingNudeBaselineOid
int EroticArmorBaselineOid
int SceneParticipantBaselineOid
int SceneViewerBaselineOid
int VictimGainsArousalOid

int DeviceBaselineGainTypeOid
int DeviceBaselineGainValueOid
int SelectedDeviceTypeId
string[] DeviceTypeNames

;Event-Based
int SceneBeginArousalOid
int StageChangeArousalOid

int OrgasmArousalLossOid

int SceneEndArousalNoOrgasmOid
int SceneEndArousalOrgasmOid

;Rate of Change
int ArousalRateOfChangeOid
int LibidoRateOfChangeOid

;Settings
int EnableStatBuffsOid
int EnableSOSIntegrationOid

;---- Puppet Properties ----
Actor Property PuppetActor Auto
int Property SetArousalOid Auto
int Property SetLibidoOid Auto
int Property SetArousalMultiplierOid Auto

float Property kDefaultArousalMultiplier = 1.0 AutoReadOnly

;------- UI Page ---------
int CheckArousalKeyOid
int ArousalBarXOid
int ArousalBarYOid
int ArousalBarDisplayModeOid
string[] ArousalBarDisplayModeNames
int ArousalBarToggleKeyOid

;------ System Properties -------
int DumpArousalData
int ClearSecondaryArousalData
int ClearAllArousalData

int EnableDebugModeOid

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

;------ Baseline Explain -------
int ExplainNakedOid
int ExplainSpectatingNakedOid

;------- Help ---------
int HelpOverviewOid
int HelpCurrentArousalOid
int HelpBaselineArousalOid
int HelpLibidoOid

int HelpGainArousalOid
int HelpLowerArousalOid
int HelpGainBaselineOid
int HelpLowerBaselineOid

int function GetVersion()
    return 205 ; 2.0.5
endfunction

Event OnConfigInit()
    ModName = "OSLAroused"

    Pages = new String[8]
    Pages[0] = "Overview"
    Pages[1] = "Puppeteer"
    Pages[2] = "Keywords"
    Pages[3] = "UI/Notifications"
    Pages[4] = "Settings"
    Pages[5] = "System"
    Pages[6] = "Baseline Status"
    Pages[7] = "Help"


    ArousalBarDisplayModeNames = new String[4]
    ArousalBarDisplayModeNames[0] = "Always Off"
    ArousalBarDisplayModeNames[1] = "Fade"
    ArousalBarDisplayModeNames[2] = "Toggle Showing"
    ArousalBarDisplayModeNames[3] = "Always On"
EndEvent

Event OnVersionUpdate(Int NewVersion)
    If (CurrentVersion != 0)
        OnConfigInit()

        if (CurrentVersion < 205)
            Main.SetDeviceTypeBaselineChange(15, 0)
            Main.SetDeviceTypeBaselineChange(16, 5)
        endif
    EndIf
EndEvent

Event OnGameLoaded()
    parent.OnGameReload()

    PuppetActor = Game.GetPlayer()
    
	EroticArmorKeyword = Keyword.GetKeyword("EroticArmor")
	BikiniArmorKeyword = Keyword.GetKeyword("_SLS_BikiniArmor")
	SLAArmorPrettyKeyword = Keyword.GetKeyword("SLA_ArmorPretty")
	SLAArmorHalfNakedKeyword = Keyword.GetKeyword("SLA_ArmorHalfNaked")
	SLAArmorSpendexKeyword = Keyword.GetKeyword("SLA_ArmorSpendex")
	SLAHasStockingsKeyword = Keyword.GetKeyword("SLA_HasStockings")
EndEvent

Event OnPageReset(string page)
    PrintActiveDevices()
    SetCursorFillMode(TOP_TO_BOTTOM)
    if(page == "Overview")
        OverviewLeftColumn()
        SetCursorPosition(1)
        RenderActorStatus(PuppetActor)
    elseif(page == "Puppeteer")
        PuppeteerPage(PuppetActor)
    elseif(page == "Keywords")
        KeywordPage()
    elseif(page == "UI/Notifications")
        UIPage()
    elseif(page == "Settings")
        SettingsLeftColumn()
        SetCursorPosition(1)
        SettingsRightColumn()
    elseif(page == "System")
        SystemPage()
    elseif(page == "Baseline Status")
        BaselineStatusPage()
    elseif(page == "Help")
        HelpPage()
    endif
EndEvent

function OverviewLeftColumn()
    AddHeaderOption("OSL Aroused Status")
    AddTextOption("OSL Aroused Is:", "Enabled")
    AddEmptyOption()
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
    AddEmptyOption()
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

function RenderActorStatus(Actor target)
    if(target == none)
        AddHeaderOption("No Target Selected")
        return
    endif
    AddHeaderOption(target.GetDisplayName())

    ArousalStatusOid = AddTextOption("Current Arousal", OSLArousedNative.GetArousal(target))
    BaselineArousalStatusOid = AddTextOption("Baseline Arousal", OSLArousedNative.GetArousalBaseline(target))
    LibidoStatusOid = AddTextOption("Libido", OSLArousedNative.GetLibido(target))
    ArousalMultiplierStatusOid = AddTextOption("Arousal Multiplier", OSLArousedNative.GetArousalMultiplier(target))
endfunction

function PuppeteerPage(Actor target)
    if(target == none)
        AddHeaderOption("No Target Selected")
        return
    endif
    AddHeaderOption(target.GetLeveledActorBase().GetName())

    float arousal = OSLArousedNative.GetArousal(PuppetActor)
    SetArousalOid = AddSliderOption("Arousal", ((arousal * 100) / 100) as int, "{1}")

    float libido = OSLArousedNative.GetLibido(PuppetActor)
    SetLibidoOid = AddSliderOption("Libido", ((libido * 100) / 100) as int, "{1}")
    
    float arousalMultiplier = OSLArousedNative.GetArousalMultiplier(PuppetActor)
    SetArousalMultiplierOid = AddSliderOption("Arousal Multiplier", arousalMultiplier, "{1}")
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

function UIPage()
    CheckArousalKeyOid = AddKeyMapOption("Show Arousal Key", Main.GetShowArousalKeybind())

    AddHeaderOption("Arousal Bar")
    ArousalBarXOid = AddSliderOption("X Pos", Main.ArousalBar.X)
    ArousalBarYOid = AddSliderOption("Y Pos", Main.ArousalBar.Y)
    ArousalBarDisplayModeOid = AddMenuOption("Display Mode", ArousalBarDisplayModeNames[Main.ArousalBar.DisplayMode])

    ArousalBarToggleKeyOid = AddKeyMapOption("Toggle Key", Main.GetToggleArousalBarKeybind())
endfunction

function SettingsLeftColumn()
    EnableStatBuffsOid = AddToggleOption("Enable Arousal Stat (De)Buffs", Main.EnableArousalStatBuffs)
    EnableSOSIntegrationOid = AddToggleOption("Enable SOS Integration", Main.EnableSOSIntegration)

    AddHeaderOption("Baseline Arousal Gains")
    SceneParticipantBaselineOid = AddSliderOption("Participating In Sex", Main.SceneParticipationBaselineIncrease, "{1}")
    VictimGainsArousalOid = AddToggleOption("Victim Gains Arousal", Main.VictimGainsArousal)
    SceneViewerBaselineOid = AddSliderOption("Spectating Sex", Main.SceneViewingBaselineIncrease, "{1}")
    BeingNudeBaselineOid = AddSliderOption("Being Nude", Main.NudityBaselineIncrease, "{1}")
    ViewingNudeBaselineOid = AddSliderOption("Viewing Nude", Main.ViewingNudityBaselineIncrease, "{1}")
    EroticArmorBaselineOid = AddSliderOption("Wearing Erotic Armor", Main.EroticArmorBaselineIncrease, "{1}")
    AddHeaderOption("Device Baseline Gains")
    DeviceBaselineGainTypeOid = AddMenuOption("Device Type", "")
    DeviceBaselineGainValueOid = AddSliderOption("Selected Type Gain", 0)

endfunction

function SettingsRightColumn()
    AddHeaderOption("Event-Based Arousal Gains")
    SceneBeginArousalOid = AddSliderOption("Sex Scene Begin", Main.SceneBeginArousalGain, "{1}")
    StageChangeArousalOid = AddSliderOption("Sex Stage Change", Main.StageChangeArousalGain, "{1}")
    OrgasmArousalLossOid = AddSliderOption("Orgasm Arousal Loss", -Main.OrgasmArousalChange, "{1}")
    SceneEndArousalNoOrgasmOid = AddSliderOption("Sex Scene End (No Orgasm)", Main.SceneEndArousalNoOrgasmChange, "{1}")
    SceneEndArousalOrgasmOid = AddSliderOption("Sex Scene End (SLSO Orgasm)", Main.SceneEndArousalOrgasmChange, "{1}")
    AddHeaderOption("Attribute Change Rates")
    ArousalRateOfChangeOid = AddSliderOption("Arousal Rate of Change", Main.ArousalChangeRate, "{1}")
    LibidoRateOfChangeOid = AddSliderOption("Libido Rate of Change", Main.LibidoChangeRate, "{1}")

endfunction

function SystemPage()
    AddHeaderOption("Native Data")
    DumpArousalData = AddTextOption("Dump Arousal Data", "RUN")
    ClearAllArousalData = AddTextOption("Clear All Arousal Data", "RUN")
    EnableDebugModeOid = AddToggleOption("Enable Debug Logging", Main.EnableDebugMode)
endfunction

function BaselineStatusPage()
    AddHeaderOption("Baseline Arousal Contributions")
    if(OSLArousedNative.IsNaked(PuppetActor))
        AddTextOption("Actor Naked", Main.NudityBaselineIncrease)
        AddTextOption("Viewing Nude Actor", "0")
    elseif (OSLArousedNative.IsViewingNaked(PuppetActor))
        AddTextOption("Actor Naked", "0")
        AddTextOption("Viewing Nude Actor", Main.ViewingNudityBaselineIncrease)
    else
        AddTextOption("Actor Naked", "0")
        AddTextOption("Viewing Nude Actor", "0")
    endif

    if(OSLArousedNative.IsInScene(PuppetActor))
        AddTextOption("In Scene", Main.SceneParticipationBaselineIncrease)
        AddTextOption("Viewing Sex Scene", "0")
    elseif (OSLArousedNative.IsViewingScene(PuppetActor))
        AddTextOption("In Scene", "0")
        AddTextOption("Viewing Sex Scene", Main.SceneViewingBaselineIncrease)
    else
        AddTextOption("In Scene", "0")
        AddTextOption("Viewing Sex Scene", "0")
    endif

    if(OSLArousedNative.IsWearingEroticArmor(PuppetActor))
        AddTextOption("Worn Erotic Armor", Main.EroticArmorBaselineIncrease)
    else
        AddTextOption("Worn Erotic Armor", "0")
    endif

    AddTextOption("Worn Devices Gain", OSLArousedNative.WornDeviceBaselineGain(PuppetActor))
endfunction

function HelpPage()
    AddHeaderOption("OSL Aroused Help Topics")

    HelpOverviewOid = AddTextOption("Overview", "Click To Read")
    HelpCurrentArousalOid = AddTextOption("Current Arousal", "Click To Read")
    HelpBaselineArousalOid = AddTextOption("Baseline Arousal", "Click To Read")
    HelpLibidoOid = AddTextOption("Libido", "Click To Read")

    SetCursorPosition(1)

    HelpGainArousalOid = AddTextOption("Gain Arousal", "Click To Read")
    HelpLowerArousalOid = AddTextOption("Lower Arousal", "Click To Read")
    HelpGainBaselineOid = AddTextOption("Raise Baseline", "Click To Read")
    HelpLowerBaselineOid = AddTextOption("Lower Arousal", "Click To Read")
endfunction

event OnOptionSelect(int optionId)
    if(CurrentPage == "Settings")
        if(optionId == VictimGainsArousalOid)
            Main.VictimGainsArousal = !Main.VictimGainsArousal
            SetToggleOptionValue(VictimGainsArousalOid, Main.VictimGainsArousal)
        elseif(optionId == EnableStatBuffsOid)
            Main.SetArousalEffectsEnabled(!Main.EnableArousalStatBuffs) 
            SetToggleOptionValue(EnableStatBuffsOid, Main.EnableArousalStatBuffs)
        elseif(optionId == EnableSOSIntegrationOid)
            Main.EnableSOSIntegration = !Main.EnableSOSIntegration
            SetToggleOptionValue(EnableSOSIntegrationOid, Main.EnableSOSIntegration)
        endif
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
    ElseIf(CurrentPage == "Help")
        if(optionId == HelpOverviewOid)
            Debug.MessageBox("OSL Aroused uses a robust Arousal Management system, where both \"state\" and \"event\" based arousal modification is supported, Baseline Arousal represents the state of arousal the player \"wants\" to be in. And their Current Arousal will gradually move towards that value. Certain events can directly manipulate your arousal, which will gradually \"normalize\" back to baseline")
        elseif(optionId == HelpCurrentArousalOid)
            Debug.MessageBox("Current Arousal represents your overall arousal level in range (0-100). It will Gradually move towards your Baseline Arousal over time. Certain Events/Other Mods can increase/decrease your current arousal, and it will gradually normalize back towards your Baseline Arousal.")
        elseif(optionId == HelpBaselineArousalOid)
            Debug.MessageBox("Baseline Arousal represents a baseline arousal level that your current arousal wants to move towards. It is Modified by certain states (such as erotic clothing, worn devices, nudity/having sex, Libido etc..)")
        elseif(optionId == HelpLibidoOid)
            Debug.MessageBox("Libido represets the minimum value your BaselineArousal can drop to (excluding arousal supression states). It will very slowly move towards your Current Arousal value. Remaining at high arousal will increase your libido towards that arousal value, and to lower libido you need to keep your arousal below your current libido value")
        elseif(optionId == HelpGainArousalOid)
            Debug.MessageBox("Arousal Gains can be triggered from other Mods. Additionally, OSLAroused will cause direct arousal gains when a sex scene begins, and whenever the stage is changed.")
        elseif(optionId == HelpLowerArousalOid)
            Debug.MessageBox("Arousal Reduction can be triggered from other Mods. Additionally, OSLAroused will cause direct arousal Reduction when an actor orgasms.")
        elseif(optionId == HelpGainBaselineOid)
            Debug.MessageBox("Baseline Is Raised From Being Nude, Being Near Nude Actors, Partcipating in Sex, Spectating Sex and Wearing EroticArmor.")
        elseif(optionId == HelpLowerBaselineOid)
            Debug.MessageBox("Baseline can be reduced by removing any equipment/states that raise baseline (ex. Clothes, Devices etc). Baseline is actively supressed when the player is outside in the rain.")
        endif
    EndIf
endevent

Event OnOptionKeyMapChange(int optionId, int keyCode, string conflictControl, string conflictName)
    if(optionId == CheckArousalKeyOid)
        Main.SetShowArousalKeybind(keyCode)
        SetKeyMapOptionValue(CheckArousalKeyOid, keyCode)
    elseif(optionId == ArousalBarToggleKeyOid)
        Main.SetToggleArousalBarKeybind(keyCode)
        SetKeyMapOptionValue(ArousalBarToggleKeyOid, keyCode)
    endif
EndEvent

event OnOptionHighlight(int optionId)
    if(optionId == ArousalStatusOid)
        SetInfoText("Actors current Arousal ranging [0-100]. Will gradually move towards your baseline arousal.")
    elseif(optionId == BaselineArousalStatusOid)
        SetInfoText("Target value for arousal to move towards. Based off Libido plus any additonal effects (ex. Lewd Clothing, Nudity, Devious Devices, Participating/viewing adult scenes)")
    elseif(optionId == LibidoStatusOid)
        SetInfoText("Base arousal before any effects. Will very slowly move towards actor arousal over time. Keep arousal low to reduce.")
    elseif(optionId == ArousalMultiplierStatusOid)
        SetInfoText("Multiplier applied to any Arousal Gains")
    endif
    
    if(CurrentPage == "Overview")
        if(optionId == SLAStubLoadedOid)
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
    elseif(CurrentPage == "UI/Notifications")
        if(optionId == ArousalBarToggleKeyOid)
            SetInfoText("Key To Toggle Arousal Bar Display when in Toggle Mode")
        elseif(optionId == CheckArousalKeyOid)
            SetInfoText("Key To Show Arousal Info")
        endif
    elseif(CurrentPage == "Settings")
        if(optionId == EnableStatBuffsOid)
            SetInfoText("Will Enable Arousal based Stat Buffs")
        elseif(optionId == EnableSOSIntegrationOid)
            SetInfoText("Will Enable Arousal effecting shrong state. Requires SOS to be installed")
        elseif(optionId == BeingNudeBaselineOid)
            SetInfoText("Amount Baseline Arousal is increased by when nude")
        elseif(optionId == ViewingNudeBaselineOid)
            SetInfoText("Amount Baseline Arousal is increased by when near naked NPCs")
        elseif(optionId == EroticArmorBaselineOid)
            SetInfoText("Amount Baseline Arousal is increased by when nearing Armor with EroticArmor keyword")
        elseif(optionId == SceneParticipantBaselineOid)
            SetInfoText("Amount Baseline Arousal is increased by when participating in scene")
        elseif(optionId == SceneViewerBaselineOid)
            SetInfoText("Baseline increase when Spectating scene")
        elseif(optionId == VictimGainsArousalOid)
            SetInfoText("Should gain Arousal/Baseline when Victim?")
        elseif(optionId == SceneBeginArousalOid)
            SetInfoText("Amount of Arousal gained when scene starts")
        elseif(optionId == StageChangeArousalOid)
            SetInfoText("Amount of Arousal gained when scene stage changes")
        elseif(optionId == OrgasmArousalLossOid)
            SetInfoText("Amount of Arousal lost when Actor Orgasms")
        elseif(optionId == SceneEndArousalNoOrgasmOid)
            SetInfoText("Amount of Arousal Changed when Actor scene ends and actor did not orgasm (or SLSO not installed). [And not victim when VictimGainsArousal is false]")
        elseif(optionId == SceneEndArousalOrgasmOid)
            SetInfoText("Amount of Arousal Changed when Actor scene ends and actor did orgasm (and SLSO installed)")
        elseif(optionId == ArousalRateOfChangeOid)
            SetInfoText("Percentage of Difference Arousal moves towards Baseline after 1 ingame hour. Ex. Rate 50, Arousal 100, Baseline 50, Arousal is 75 after 1 hour, 62.5 after 2, 56.25 after 3, etc...")
        elseif(optionId == LibidoRateOfChangeOid)
            SetInfoText("Percentage of Difference Libido moves towards Arousal after 1 ingame hour. Ex. Rate 10, Arousal 100, Libido 0, Libido is 10 after 1 hour, 19 after 2, 27.1 after 3, etc...")
        endif
    elseif(CurrentPage == "System")
        if(optionId == DumpArousalData)
            SetInfoText("Dump all stored arousal data to SKSE log file")
        elseif(optionId == ClearSecondaryArousalData)
            SetInfoText("Clear NPC Arousal data from Save (This Maintains Player/Unique Data)")
        elseif(optionId == ClearAllArousalData)
            SetInfoText("Clear All Arousal data from Save")
        endif
    EndIf
endevent

event OnOptionMenuOpen(int optionId)
    if (CurrentPage == "Keywords")
        if(optionId == ArmorListMenuOid)
            LoadArmorList()
        endif
    elseif (CurrentPage == "UI/Notifications")
        if(optionId == ArousalBarDisplayModeOid)
            SetMenuDialogStartIndex(Main.ArousalBar.DisplayMode)
            SetMenuDialogDefaultIndex(1)
            SetMenuDialogOptions(ArousalBarDisplayModeNames)
        endif
    elseif (CurrentPage == "Settings")
        if(optionId == DeviceBaselineGainTypeOid)
            LoadDeviceTypesList();
        endif
    endif
endevent

event OnOptionMenuAccept(int optionId, int index)
    If (CurrentPage == "Keywords")
        If (optionId == ArmorListMenuOid)
            Form[] equippedArmor = OSLArousedNativeActor.GetAllEquippedArmor(Game.GetPlayer())
            SelectedArmor = equippedArmor[FoundArmorIds[index]] as Armor
            SetMenuOptionValue(optionId, FoundArmorNames[index])
            ArmorSelected()
        EndIf
    elseif (CurrentPage == "UI/Notifications")
        if(optionId == ArousalBarDisplayModeOid)
            Main.ArousalBar.SetDisplayMode(index)
            SetMenuOptionValue(optionId, ArousalBarDisplayModeNames[index])
        endif
    elseif (CurrentPage == "Settings")
        if(optionId == DeviceBaselineGainTypeOid)
            SelectedDeviceTypeId = index
            SetMenuOptionValue(optionId, DeviceTypeNames[index])
            SetSliderOptionValue(DeviceBaselineGainValueOid, Main.DeviceBaselineModifications[index])
        endif
    endif
endevent

event OnOptionSliderOpen(int option)
    if(CurrentPage == "Puppeteer")
        if(option == SetArousalOid)
            float arousal = 0
            arousal = OSLArousedNative.GetArousal(PuppetActor)
            SetSliderDialogStartValue(arousal)
            SetSliderDialogDefaultValue(0)
            SetSliderDialogRange(0, 100)
            SetSliderDialogInterval(1)
        elseif (option == SetLibidoOid)
            float libido = OSLArousedNative.GetLibido(PuppetActor)
            SetSliderDialogStartValue(libido)
            SetSliderDialogDefaultValue(0)
            SetSliderDialogRange(0, 100)
            SetSliderDialogInterval(1)
        ElseIf (option == SetArousalMultiplierOid)
            float arousalMultiplier = OSLArousedNative.GetArousalMultiplier(PuppetActor)
            SetSliderDialogStartValue(arousalMultiplier)
            SetSliderDialogDefaultValue(kDefaultArousalMultiplier)
            SetSliderDialogRange(0, 10)
            SetSliderDialogInterval(0.2)
        endif
    ElseIf(CurrentPage == "UI/Notifications")
        if(option == ArousalBarXOid)
            SetSliderDialogStartValue(Main.ArousalBar.X)
            SetSliderDialogDefaultValue(980)
            SetSliderDialogRange(0, 1000)
        elseif(option == ArousalBarYOid)
            SetSliderDialogStartValue(Main.ArousalBar.Y)
            SetSliderDialogDefaultValue(160)
            SetSliderDialogRange(35, 710)
        endif
    elseIf (currentPage == "Settings")
        if(option == SceneParticipantBaselineOid)
            SetSliderDialogStartValue(Main.SceneParticipationBaselineIncrease)
            SetSliderDialogDefaultValue(50)
            SetSliderDialogRange(0, 100)
        elseif(option == SceneViewerBaselineOid)
            SetSliderDialogStartValue(Main.SceneViewingBaselineIncrease)
            SetSliderDialogDefaultValue(20)
            SetSliderDialogRange(0, 100)
        elseif(option == BeingNudeBaselineOid)
            SetSliderDialogStartValue(Main.NudityBaselineIncrease)
            SetSliderDialogDefaultValue(30)
            SetSliderDialogRange(0, 50)
        elseif(option == ViewingNudeBaselineOid)
            SetSliderDialogStartValue(Main.ViewingNudityBaselineIncrease)
            SetSliderDialogDefaultValue(20)
            SetSliderDialogRange(0, 50)
        elseif(option == EroticArmorBaselineOid)
            SetSliderDialogStartValue(Main.EroticArmorBaselineIncrease)
            SetSliderDialogDefaultValue(20)
            SetSliderDialogRange(0, 50)
        elseif(option == DeviceBaselineGainValueOid)
            SetSliderDialogStartValue(Main.DeviceBaselineModifications[SelectedDeviceTypeId])
            SetSliderDialogDefaultValue(Main.DeviceBaselineModifications[SelectedDeviceTypeId])
            SetSliderDialogRange(0, 50)
        elseif(option == SceneBeginArousalOid)
            SetSliderDialogStartValue(Main.SceneBeginArousalGain)
            SetSliderDialogDefaultValue(10)
            SetSliderDialogRange(0, 50)
        elseif(option == StageChangeArousalOid)
            SetSliderDialogStartValue(Main.StageChangeArousalGain)
            SetSliderDialogDefaultValue(3)
            SetSliderDialogRange(0, 20)
        elseif(Option == OrgasmArousalLossOid)
            SetSliderDialogStartValue(-Main.OrgasmArousalChange)
            SetSliderDialogDefaultValue(50)
            SetSliderDialogRange(0, 100)
        elseif(Option == SceneEndArousalNoOrgasmOid)
            SetSliderDialogStartValue(Main.SceneEndArousalNoOrgasmChange)
            SetSliderDialogDefaultValue(-40)
            SetSliderDialogRange(-100, 50)
        elseif(Option == SceneEndArousalOrgasmOid)
            SetSliderDialogStartValue(Main.SceneEndArousalOrgasmChange)
            SetSliderDialogDefaultValue(0)
            SetSliderDialogRange(-100, 50)
        elseif(option == ArousalRateOfChangeOid)
            SetSliderDialogStartValue(Main.ArousalChangeRate)
            SetSliderDialogDefaultValue(50)
            SetSliderDialogRange(0, 100)
        elseif(option == LibidoRateOfChangeOid)
            SetSliderDialogStartValue(Main.LibidoChangeRate)
            SetSliderDialogDefaultValue(10)
            SetSliderDialogInterval(0.5)
            SetSliderDialogRange(0, 50)
        endif
    endif
endevent

event OnOptionSliderAccept(int option, float value)
    if(currentPage == "Puppeteer")
        if(option == SetArousalOid)
            OSLArousedNative.SetArousal(PuppetActor, value)
            SetSliderOptionValue(SetArousalOid, value, "{1}")
        elseif(option == SetLibidoOid)
            OSLArousedNative.SetLibido(PuppetActor, value)
            SetSliderOptionValue(SetLibidoOid, value, "{1}")
        elseif(option == SetArousalMultiplierOid)
            OSLArousedNative.SetArousalMultiplier(PuppetActor, value)
            SetSliderOptionValue(SetArousalMultiplierOid, value, "{1}")
        endif
    elseif(currentPage == "UI/Notifications")
        if(option == ArousalBarXOid)
            Main.ArousalBar.SetPosX(value)
        elseif(option == ArousalBarYOid)
            Main.ArousalBar.SetPosY(value)
        endif
        SetSliderOptionValue(option, value)
    elseIf (currentPage == "Settings")
        if(option == SceneParticipantBaselineOid)
            Main.SetSceneParticipantBaseline(value)
            SetSliderOptionValue(SceneParticipantBaselineOid, value, "{1}")
        elseif(option == SceneViewerBaselineOid)
            Main.SetSceneViewingBaseline(value)
            SetSliderOptionValue(SceneViewerBaselineOid, value, "{1}")
        elseif(option == BeingNudeBaselineOid)
            Main.SetBeingNudeBaseline(value)
            SetSliderOptionValue(BeingNudeBaselineOid, value, "{1}")
        elseif(option == ViewingNudeBaselineOid)
            Main.SetViewingNudeBaseline(value)
            SetSliderOptionValue(ViewingNudeBaselineOid, value, "{1}")
        elseif(option == EroticArmorBaselineOid)
            Main.SetEroticArmorBaseline(value)
            SetSliderOptionValue(EroticArmorBaselineOid, value, "{1}")
        elseif(option == DeviceBaselineGainValueOid)
            Main.SetDeviceTypeBaselineChange(SelectedDeviceTypeId, value)
            SetSliderOptionValue(DeviceBaselineGainValueOid, value)
        elseif(option == SceneBeginArousalOid)
            Main.SceneBeginArousalGain = value
            SetSliderOptionValue(SceneParticipantBaselineOid, value, "{1}")
        elseif(option == StageChangeArousalOid)
            Main.StageChangeArousalGain = value
            SetSliderOptionValue(SceneParticipantBaselineOid, value, "{1}")
        elseif(option == OrgasmArousalLossOid)
            Main.OrgasmArousalChange = -value
            SetSliderOptionValue(OrgasmArousalLossOid, value, "{1}")
        elseif(option == SceneEndArousalNoOrgasmOid)
            Main.SceneEndArousalNoOrgasmChange = value
            SetSliderOptionValue(SceneEndArousalNoOrgasmOid, value, "{1}")
        elseif(option == SceneEndArousalOrgasmOid)
            Main.SceneEndArousalOrgasmChange = value
            SetSliderOptionValue(SceneEndArousalOrgasmOid, value, "{1}")
        elseif(option == ArousalRateOfChangeOid)
            Main.SetArousalChangeRate(value)
            SetSliderOptionValue(ArousalRateOfChangeOid, value, "{1}")
        elseif(option == LibidoRateOfChangeOid)
            Main.SetLibidoChangeRate(value)
            SetSliderOptionValue(LibidoRateOfChangeOid, value, "{1}")
        endif
    endif
endevent

event OnOptionDefault(int option)
    if(currentPage == "Puppeteer")
        if(option == SetArousalOid)
            OSLArousedNative.SetArousal(PuppetActor, 0)
            SetSliderOptionValue(SetArousalOid, 0, "{1}")
        elseif(option == SetLibidoOid)
            OSLArousedNative.SetLibido(PuppetActor, 0)
            SetSliderOptionValue(SetLibidoOid, 0, "{1}")
        elseif(option == SetArousalMultiplierOid)
            OSLArousedNative.SetArousalMultiplier(PuppetActor, kDefaultArousalMultiplier)
            SetSliderOptionValue(SetArousalMultiplierOid, kDefaultArousalMultiplier, "{1}")
        endif
    elseif(currentPage == "UI")
        if(option == ArousalBarXOid)
            Main.ArousalBar.SetPosX(980)
            SetSliderOptionValue(option, 980)
        elseif(option == ArousalBarYOid)
            Main.ArousalBar.SetPosY(160)
            SetSliderOptionValue(option, 160)
        endif
    elseif(CurrentPage == "Settings")
        if(option == SceneParticipantBaselineOid)
            Main.SetSceneParticipantBaseline(50)
            SetSliderOptionValue(SceneParticipantBaselineOid, 50, "{1}")
        elseif(option == SceneViewerBaselineOid)
            Main.SetSceneViewingBaseline(20)
            SetSliderOptionValue(SceneViewerBaselineOid, 20, "{1}")
        elseif(option == BeingNudeBaselineOid)
            Main.SetBeingNudeBaseline(30)
            SetSliderOptionValue(BeingNudeBaselineOid, 30, "{1}")
        elseif(option == ViewingNudeBaselineOid)
            Main.SetViewingNudeBaseline(20)
            SetSliderOptionValue(ViewingNudeBaselineOid, 20, "{1}")
        elseif(option == EroticArmorBaselineOid)
            Main.SetEroticArmorBaseline(20)
            SetSliderOptionValue(EroticArmorBaselineOid, 20, "{1}")
        elseif(option == SceneBeginArousalOid)
            Main.SceneBeginArousalGain = 10
            SetSliderOptionValue(SceneBeginArousalOid, 10, "{1}")
        elseif(option == StageChangeArousalOid)
            Main.StageChangeArousalGain = 3
            SetSliderOptionValue(StageChangeArousalOid, 3, "{1}")
        elseif(option == OrgasmArousalLossOid)
            Main.OrgasmArousalChange = -50
            SetSliderOptionValue(OrgasmArousalLossOid, 50, "{1}")
        elseif(option == SceneEndArousalNoOrgasmOid)
            Main.SceneEndArousalNoOrgasmChange = -40
            SetSliderOptionValue(SceneEndArousalNoOrgasmOid, -40, "{1}")
        elseif(option == SceneEndArousalOrgasmOid)
            Main.SceneEndArousalOrgasmChange = 0
            SetSliderOptionValue(SceneEndArousalOrgasmOid, 0, "{1}")
        elseif(option == ArousalRateOfChangeOid)
            Main.SetArousalChangeRate(50)
            SetSliderOptionValue(ArousalRateOfChangeOid, 50, "{1}")
        elseif(option == LibidoRateOfChangeOid)
            Main.SetLibidoChangeRate(10)
            SetSliderOptionValue(LibidoRateOfChangeOid, 10, "{1}")
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
    Log("Loading Armor List")
    SelectedArmor = none

    Actor player = Game.GetPlayer()
    Form[] equippedArmor = OSLArousedNativeActor.GetAllEquippedArmor(player)
    Log("Loaded Armor List, Playter has: " + equippedArmor.Length + " Equipped Armor")
    int index = 0
    FoundArmorNames = new string[64]
    FoundArmorIds = new int[64]
    int foundItemIndex = 0
    while(index < equippedArmor.Length && foundItemIndex < 64)
        Armor armorItem = equippedArmor[index] as Armor
        if(armorItem)
            string armorName = armorItem.GetName()
            if(armorName)
                FoundArmorNames[foundItemIndex] = armorItem.GetName()
                FoundArmorIds[foundItemIndex] = index
                foundItemIndex += 1
            endif
        endif
        index += 1
    endwhile

    FoundArmorNames = Utility.ResizeStringArray(FoundArmorNames, foundItemIndex)
    FoundArmorIds = Utility.ResizeIntArray(FoundArmorIds, foundItemIndex)
    Log("Loading Armor List, ArmorIds : " + FoundArmorIds.Length + " index: " + foundItemIndex)
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

function PrintActiveDevices()
    int[] activeDeviceTypeIds = OSLArousedNativeActor.GetActiveDeviceTypeIds(Game.GetPlayer())

    DeviceTypeNames = new string[19]
    DeviceTypeNames[0] = "Belt"
    DeviceTypeNames[1] = "Collar"
    DeviceTypeNames[2] = "LegCuffs"
    DeviceTypeNames[3] = "ArmCuffs"
    DeviceTypeNames[4] = "Bra"
    DeviceTypeNames[5] = "Gag"
    DeviceTypeNames[6] = "PiercingsNipple"
    DeviceTypeNames[7] = "PiercingsVaginal"
    DeviceTypeNames[8] = "Blindfold"
    DeviceTypeNames[9] = "Harness"
    DeviceTypeNames[10] = "PlugVaginal"
    DeviceTypeNames[11] = "PlugAnal"
    DeviceTypeNames[12] = "Corset"
    DeviceTypeNames[13] = "Boots"
    DeviceTypeNames[14] = "Gloves"
    DeviceTypeNames[15] = "Hood"
    DeviceTypeNames[16] = "Suit"
    DeviceTypeNames[17] = "HeavyBondage"
    DeviceTypeNames[18] = "BondageMittens"

    Log("Printing Active Devices:")
    int index = 0
    while(index < activeDeviceTypeIds.Length)
        Log(DeviceTypeNames[activeDeviceTypeIds[index]])
        index += 1
    endwhile
endfunction

function LoadDeviceTypesList()
    DeviceTypeNames = new string[19]
    DeviceTypeNames[0] = "Belt"
    DeviceTypeNames[1] = "Collar"
    DeviceTypeNames[2] = "LegCuffs"
    DeviceTypeNames[3] = "ArmCuffs"
    DeviceTypeNames[4] = "Bra"
    DeviceTypeNames[5] = "Gag"
    DeviceTypeNames[6] = "PiercingsNipple"
    DeviceTypeNames[7] = "PiercingsVaginal"
    DeviceTypeNames[8] = "Blindfold"
    DeviceTypeNames[9] = "Harness"
    DeviceTypeNames[10] = "PlugVaginal"
    DeviceTypeNames[11] = "PlugAnal"
    DeviceTypeNames[12] = "Corset"
    DeviceTypeNames[13] = "Boots"
    DeviceTypeNames[14] = "Gloves"
    DeviceTypeNames[15] = "Hood"
    DeviceTypeNames[16] = "Suit"
    DeviceTypeNames[17] = "HeavyBondage"
    DeviceTypeNames[18] = "BondageMittens"
    SetMenuDialogOptions(DeviceTypeNames)
endfunction

function Log(string msg) global
    Debug.Trace("---OSLAroused--- [MCM] " + msg)
endfunction
