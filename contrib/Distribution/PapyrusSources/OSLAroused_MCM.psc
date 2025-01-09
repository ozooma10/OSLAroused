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

int MinLibidoPlayerOid
int MinLibidoNPCOid
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

int GenderPreferenceOid
string[] GenderPreferenceList

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
int ClearAllArousalData

int EnableDebugModeOid

;------ Keywords -------
int ArmorListMenuOid
Armor SelectedArmor
string[] FoundArmorNames
int[] FoundArmorIds

Keyword Property EroticArmorKeyword Auto

Form[] RegisteredKeywords
bool[] RegisteredKeywordStates
int[] RegisteredKeywordOids

int RegisterKeywordOid

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
    return 260 ; 2.6.0
endfunction

Event OnConfigInit()
    ModName = "OSLAroused"

    Pages = new String[8]
    Pages[0] = "$OSL_Overview"
    Pages[1] = "$OSL_Puppeteer"
    Pages[2] = "$OSL_Keywords"
    Pages[3] = "$OSL_UI"
    Pages[4] = "$OSL_Settings"
    Pages[5] = "$OSL_System"
    Pages[6] = "$OSL_BaselineStatus"
    Pages[7] = "$OSL_Help"


    ArousalBarDisplayModeNames = new String[4]
    ArousalBarDisplayModeNames[0] = "$OSL_AlwaysOff"
    ArousalBarDisplayModeNames[1] = "$OSL_Fade"
    ArousalBarDisplayModeNames[2] = "$OSL_ToggleShow"
    ArousalBarDisplayModeNames[3] = "$OSL_AlwaysOn"

    GenderPreferenceList = new string[4]
    GenderPreferenceList[0] = "$OSL_Male"
    GenderPreferenceList[1] = "$OSL_Female"
    GenderPreferenceList[2] = "$OSL_Both"
    GenderPreferenceList[3] = "$OSL_Sexlab"
EndEvent

Event OnVersionUpdate(Int NewVersion)
    If (CurrentVersion != 0)
        OnConfigInit()

        if (CurrentVersion < 210)
            Main.SetDeviceTypeBaselineChange(15, 0)
            Main.SetDeviceTypeBaselineChange(16, 5)
            Main.SetArousalChangeRate(20)
        endif
    EndIf
EndEvent

Event OnGameLoaded()
    parent.OnGameReload()

    PuppetActor = Game.GetPlayer()
    
    EroticArmorKeyword = Keyword.GetKeyword("EroticArmor")

    string[] keywords = OSLArousedNative.GetRegisteredKeywords()
    RegisteredKeywords = Utility.CreateFormArray(keywords.Length)
    RegisteredKeywordStates  = Utility.CreateBoolArray(keywords.Length)
    RegisteredKeywordOids = Utility.CreateIntArray(keywords.Length)
    int index = 0
    while(index < keywords.Length)
        Keyword kw = Keyword.GetKeyword(keywords[index])
        RegisteredKeywords[index] = kw
        index += 1
    endwhile

    Keyword kw
EndEvent

Event OnPageReset(string page)
    SetCursorFillMode(TOP_TO_BOTTOM)
    if(page == "$OSL_Overview")
        OverviewLeftColumn()
        SetCursorPosition(1)
        RenderActorStatus(PuppetActor)
    elseif(page == "$OSL_Puppeteer")
        PuppeteerPage(PuppetActor)
    elseif(page == "$OSL_Keywords")
        KeywordPage()
    elseif(page == "$OSL_UI")
        UIPage()
    elseif(page == "$OSL_Settings")
        SettingsLeftColumn()
        SetCursorPosition(1)
        SettingsRightColumn()
    elseif(page == "$OSL_System")
        SystemPage()
    elseif(page == "$OSL_BaselineStatus")
        BaselineStatusPage()
    elseif(page == "$OSL_Help")
        HelpPage()
    endif
EndEvent

function OverviewLeftColumn()
    AddHeaderOption("$OSL_ArousedStatus")
    AddTextOption("$OSL_ArousedIs", "$OSL_Enabled")
    AddTextOption("$OSL_Version", GetVersion())
    AddEmptyOption()
    AddHeaderOption("$OSL_FrameworkAdapters")
    If (Main.SexLabAdapterLoaded)
        AddTextOption("SexLab", "$OSL_Enabled")
    Else
        AddTextOption("SexLab", "$OSL_Disabled", OPTION_FLAG_DISABLED)
    EndIf
    If (Main.OStimAdapterLoaded)
        If (Main.IsOStimLegacy)
            AddTextOption("OStim", "$OSL_Enabled")
        else
            AddTextOption("OStim Standalone", "$OSL_Enabled")
        endif
    Else
        AddTextOption("OStim", "$OSL_Disabled", OPTION_FLAG_DISABLED)
    EndIf
    AddEmptyOption()
    AddHeaderOption("$OSL_Compatibility")
    If (Main.InvalidSlaFound)
        SLAStubLoadedOid = AddTextOption("SexLab Aroused", "$OSL_InvalidInstall")
    ElseIf (Main.SlaStubLoaded)
        AddTextOption("SexLab Aroused", "$OSL_Enabled")
    Else
        SLAStubLoadedOid = AddTextOption("SexLab Aroused", "$OSL_Disabled")
    EndIf
    If (Main.InvalidOArousedFound)
        OArousedStubLoadedOid = AddTextOption("OAroused", "$OSL_InvalidInstall")
    elseIf (Main.OArousedStubLoaded)
        AddTextOption("OAroused", "$OSL_Enabled")
    Else
        OArousedStubLoadedOid = AddTextOption("OAroused", "$OSL_Disabled")
    EndIf
endfunction

function RenderActorStatus(Actor target)
    if(target == none)
        AddHeaderOption("$OSL_NoTarget")
        return
    endif
    AddHeaderOption(target.GetDisplayName())

    ArousalStatusOid = AddTextOption("$OSL_CurrentArousal", OSLArousedNative.GetArousal(target))
    BaselineArousalStatusOid = AddTextOption("$OSL_BaselineArousal", OSLArousedNative.GetArousalBaseline(target))
    LibidoStatusOid = AddTextOption("$OSL_Libido", OSLArousedNative.GetLibido(target))
    ArousalMultiplierStatusOid = AddTextOption("$OSL_ArousalMultiplier", OSLArousedNative.GetArousalMultiplier(target))

    if(Main.SlaFrameworkStub)
        int genderPrefIndex = Main.SlaFrameworkStub.GetGenderPreference(target)
        if(genderPrefIndex >= 0)
            AddTextOption("$OSL_GenderPreference", GenderPreferenceList[genderPrefIndex])
        endif
    endif
endfunction

function PuppeteerPage(Actor target)
    if(target == none)
        AddHeaderOption("$OSL_NoTarget")
        return
    endif
    AddHeaderOption(target.GetLeveledActorBase().GetName())

    float arousal = OSLArousedNative.GetArousal(PuppetActor)
    SetArousalOid = AddSliderOption("$OSL_Arousal", ((arousal * 100) / 100) as int, "{1}")

    float libido = OSLArousedNative.GetLibido(PuppetActor)
    SetLibidoOid = AddSliderOption("$OSL_Libido", ((libido * 100) / 100) as int, "{1}")
    
    float arousalMultiplier = OSLArousedNative.GetArousalMultiplier(PuppetActor)
    SetArousalMultiplierOid = AddSliderOption("$OSL_ArousalMultiplier", arousalMultiplier, "{1}")

    if(Main.SlaFrameworkStub)
        GenderPreferenceOid = AddMenuOption("$OSL_GenderPreference", GenderPreferenceList[Main.SlaFrameworkStub.GetGenderPreference(target, true)])
    endif
endfunction

function KeywordPage()
    AddHeaderOption("$OSL_KeywordManagement")
    RegisterKeywordOid = AddInputOption("Register New Keyword", "Register", 0)
    ArmorListMenuOid = AddMenuOption("$OSL_LoadArmorList", "")
    SetCursorPosition(1)
    int index = 0
    while(index < RegisteredKeywords.Length)
        Keyword kw = RegisteredKeywords[index] as Keyword

        RegisteredKeywordOids[index] = AddToggleOption((RegisteredKeywords[index] as Keyword).GetString(), RegisteredKeywordStates[index], OPTION_FLAG_DISABLED)
        index += 1
    endwhile
endfunction

function UIPage()
    CheckArousalKeyOid = AddKeyMapOption("$OSL_ShowArousalKey", Main.GetShowArousalKeybind())

    AddHeaderOption("$OSL_ArousalBar")
    ArousalBarXOid = AddSliderOption("$OSL_XPos", Main.ArousalBar.X)
    ArousalBarYOid = AddSliderOption("$OSL_YPos", Main.ArousalBar.Y)
    ArousalBarDisplayModeOid = AddMenuOption("$OSL_DisplayMode", ArousalBarDisplayModeNames[Main.ArousalBar.DisplayMode])

    ArousalBarToggleKeyOid = AddKeyMapOption("$OSL_ToggleKey", Main.GetToggleArousalBarKeybind())
endfunction

function SettingsLeftColumn()
    EnableStatBuffsOid = AddToggleOption("$OSL_EnableStat", Main.EnableArousalStatBuffs)
    EnableSOSIntegrationOid = AddToggleOption("$OSL_EnableSOS", Main.EnableSOSIntegration)
    MinLibidoPlayerOid = AddSliderOption("$OSL_MinLibidoPlayer", Main.MinLibidoValuePlayer, "{1}")
    MinLibidoNPCOid = AddSliderOption("$OSL_MinLibidoNPC", Main.MinLibidoValueNPC, "{1}")

    AddHeaderOption("$OSL_BaselineArousalGains")
    SceneParticipantBaselineOid = AddSliderOption("$OSL_Participating", Main.SceneParticipationBaselineIncrease, "{1}")
    VictimGainsArousalOid = AddToggleOption("$OSL_VictimGains", Main.VictimGainsArousal)
    SceneViewerBaselineOid = AddSliderOption("$OSL_Spectating", Main.SceneViewingBaselineIncrease, "{1}")
    BeingNudeBaselineOid = AddSliderOption("$OSL_Nude", Main.NudityBaselineIncrease, "{1}")
    ViewingNudeBaselineOid = AddSliderOption("$OSL_ViewingNude", Main.ViewingNudityBaselineIncrease, "{1}")
    EroticArmorBaselineOid = AddSliderOption("$OSL_EroticArmor", Main.EroticArmorBaselineIncrease, "{1}")
    AddHeaderOption("$OSL_DeviceGains")
    DeviceBaselineGainTypeOid = AddMenuOption("$OSL_DeviceType", "")
    DeviceBaselineGainValueOid = AddSliderOption("$OSL_SelectedTypeGain", 0)

endfunction

function SettingsRightColumn()
    AddHeaderOption("$OSL_EventBasedGains")
    SceneBeginArousalOid = AddSliderOption("$OSL_SceneBegin", Main.SceneBeginArousalGain, "{1}")
    StageChangeArousalOid = AddSliderOption("$OSL_SceneChange", Main.StageChangeArousalGain, "{1}")
    OrgasmArousalLossOid = AddSliderOption("$OSL_OrgasmLoss", -Main.OrgasmArousalChange, "{1}")
    SceneEndArousalNoOrgasmOid = AddSliderOption("$OSL_SceneEndNoOrgasm", Main.SceneEndArousalNoOrgasmChange, "{1}")
    SceneEndArousalOrgasmOid = AddSliderOption("$OSL_SceneEndSLSO", Main.SceneEndArousalOrgasmChange, "{1}")
    AddHeaderOption("$OSL_AttributeChange")
    ArousalRateOfChangeOid = AddSliderOption("$OSL_ArousalRate", Main.ArousalChangeRate, "{1}")
    LibidoRateOfChangeOid = AddSliderOption("$OSL_LibidoRate", Main.LibidoChangeRate, "{1}")

endfunction

function SystemPage()
    AddHeaderOption("$OSL_NativeData")
    DumpArousalData = AddTextOption("$OSL_DumpData", "RUN")
    ClearAllArousalData = AddTextOption("$OSL_ClearData", "RUN")
    EnableDebugModeOid = AddToggleOption("$OSL_EnableLogging", Main.EnableDebugMode)
endfunction

function BaselineStatusPage()
    AddHeaderOption("$OSL_BaselineContributions")
    if(OSLArousedNative.IsNaked(PuppetActor))
        AddTextOption("$OSL_Nude", Main.NudityBaselineIncrease)
        AddTextOption("$OSL_ViewingNude", "0")
    elseif (OSLArousedNative.IsViewingNaked(PuppetActor))
        AddTextOption("$OSL_Nude", "0")
        AddTextOption("$OSL_ViewingNude", Main.ViewingNudityBaselineIncrease)
    else
        AddTextOption("$OSL_Nude", "0")
        AddTextOption("$OSL_ViewingNude", "0")
    endif

    if(OSLArousedNative.IsInScene(PuppetActor))
        AddTextOption("$OSL_Participating", Main.SceneParticipationBaselineIncrease)
        AddTextOption("$OSL_Spectating", "0")
    elseif (OSLArousedNative.IsViewingScene(PuppetActor))
        AddTextOption("$OSL_Participating", "0")
        AddTextOption("$OSL_Spectating", Main.SceneViewingBaselineIncrease)
    else
        AddTextOption("$OSL_Participating", "0")
        AddTextOption("$OSL_Spectating", "0")
    endif

    if(OSLArousedNative.IsWearingEroticArmor(PuppetActor))
        AddTextOption("$OSL_EroticArmor", Main.EroticArmorBaselineIncrease)
    else
        AddTextOption("$OSL_EroticArmor", "0")
    endif

    AddTextOption("$OSL_WornDevicesGain", OSLArousedNative.WornDeviceBaselineGain(PuppetActor))

    SetCursorPosition(1)
    int[] activeDeviceTypeIds = OSLArousedNativeActor.GetActiveDeviceTypeIds(Game.GetPlayer())
    if(activeDeviceTypeIds.Length > 0)
        AddHeaderOption("$OSL_DetectedDevices")
        string[] deviceTypeNamesList = GetDeviceTypeNames()
        int index = 0
        while(index < activeDeviceTypeIds.Length)
            AddTextOption(deviceTypeNamesList[activeDeviceTypeIds[index]], Main.DeviceBaselineModifications[activeDeviceTypeIds[index]])
            index += 1
        endwhile
    endif

endfunction

function HelpPage()
    AddHeaderOption("$OSL_HelpTopics")

    HelpOverviewOid = AddTextOption("$OSL_Overview", "$OSL_ClickToRead")
    HelpCurrentArousalOid = AddTextOption("$OSL_CurrentArousal", "$OSL_ClickToRead")
    HelpBaselineArousalOid = AddTextOption("$OSL_BaselineArousal", "$OSL_ClickToRead")
    HelpLibidoOid = AddTextOption("$OSL_Libido", "$OSL_ClickToRead")

    SetCursorPosition(1)

    HelpGainArousalOid = AddTextOption("$OSL_GainArousal", "$OSL_ClickToRead")
    HelpLowerArousalOid = AddTextOption("$OSL_LowerArousal", "$OSL_ClickToRead")
    HelpGainBaselineOid = AddTextOption("$OSL_RaiseBaseline", "$OSL_ClickToRead")
    HelpLowerBaselineOid = AddTextOption("$OSL_LowerBaseline", "$OSL_ClickToRead")
endfunction

event OnOptionSelect(int optionId)
    if(CurrentPage == "$OSL_Settings")
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
    ElseIf (CurrentPage == "$OSL_Keywords")
        int index = 0
        bool bBreak = false
        while((index < RegisteredKeywordOids.Length) && !bBreak)
            if(optionId == RegisteredKeywordOids[index])
                if(RegisteredKeywordStates[index])
                    bool removeSuccess = OSLArousedNative.RemoveKeywordFromForm(SelectedArmor, RegisteredKeywords[index] as Keyword)
                    RegisteredKeywordStates[index] = !removeSuccess ;if remove success fails, indicate keyword still on
                else
                    bool updateSuccess = OSLArousedNative.AddKeywordToForm(SelectedArmor, RegisteredKeywords[index] as Keyword)
                    RegisteredKeywordStates[index] = updateSuccess
                endif
                SetToggleOptionValue(RegisteredKeywordOids[index], RegisteredKeywordStates[index])
                bBreak = true
            endif
            index += 1
        endwhile
    ElseIf(CurrentPage == "$OSL_System")
        if(optionId == DumpArousalData)
            OSLArousedNative.DumpArousalData()
        elseif(optionId == ClearAllArousalData)
            if (ShowMessage("$OSL_ClearDataConfirm"))
                OSLArousedNative.ClearAllArousalData()
            endif
        ElseIf (optionId == EnableDebugModeOid)
            Main.EnableDebugMode = !Main.EnableDebugMode
            SetToggleOptionValue(EnableDebugModeOid, Main.EnableDebugMode)
        endif
    ElseIf(CurrentPage == "$OSL_Help")
        if(optionId == HelpOverviewOid)
            Debug.MessageBox("$OSL_HelpOverview")
        elseif(optionId == HelpCurrentArousalOid)
            Debug.MessageBox("$OSL_HelpArousal")
        elseif(optionId == HelpBaselineArousalOid)
            Debug.MessageBox("$OSL_HelpBaseline")
        elseif(optionId == HelpLibidoOid)
            Debug.MessageBox("$OSL_HelpLibido")
        elseif(optionId == HelpGainArousalOid)
            Debug.MessageBox("$OSL_HelpGainArousal")
        elseif(optionId == HelpLowerArousalOid)
            Debug.MessageBox("$OSL_HelpLowerArousal")
        elseif(optionId == HelpGainBaselineOid)
            Debug.MessageBox("$OSL_HelpRaiseBaseline")
        elseif(optionId == HelpLowerBaselineOid)
            Debug.MessageBox("$OSL_HelpLowerBaseline")
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
        SetInfoText("$OSL_InfoArousal")
    elseif(optionId == BaselineArousalStatusOid)
        SetInfoText("$OSL_InfoBaseline")
    elseif(optionId == LibidoStatusOid)
        SetInfoText("$OSL_InfoLibido")
    elseif(optionId == ArousalMultiplierStatusOid)
        SetInfoText("$OSL_InfoMultiplier")
    endif
    
    if(CurrentPage == "$OSL_Overview")
        if(optionId == SLAStubLoadedOid)
            If (Main.InvalidSlaFound)
                SetInfoText("$OSL_InfoInvalidSla")
            elseif(!Main.SlaStubLoaded)
                SetInfoText("$OSL_InfoDisabledSla")
            EndIf
        elseif(optionId == OArousedStubLoadedOid)
            If (Main.InvalidOArousedFound)
                SetInfoText("$OSL_InfoInvalidOAroused")
            elseif(!Main.OArousedStubLoaded)
                SetInfoText("$OSL_InfoDisabledOAroused")
            EndIf
        endif
    elseif(CurrentPage == "$OSL_Puppeteer")
        if(optionId == GenderPreferenceOid)
            SetInfoText("$OSL_InfoGenderPreference")
        endif
    elseif(CurrentPage == "$OSL_UI")
        if(optionId == ArousalBarToggleKeyOid)
            SetInfoText("$OSL_InfoArousalToggle")
        elseif(optionId == CheckArousalKeyOid)
            SetInfoText("$OSL_InfoCheckArousal")
        endif
    elseif(CurrentPage == "$OSL_Keywords")
        if(optionId == RegisterKeywordOid)
            SetInfoText("Enter the Editor Id of the Keyword you wish to register to allow Adding/Removing from armor. (ex. SLA_ArmorPretty, SLA_ArmorSpendex, SLA_HasStockings, etc..)")
        endif
    elseif(CurrentPage == "$OSL_Settings")
        if(optionId == EnableStatBuffsOid)
            SetInfoText("$OSL_InfoStat")
        elseif(optionId == EnableSOSIntegrationOid)
            SetInfoText("$OSL_InfoSOS")
        elseif(optionId == MinLibidoPlayerOid)
            SetInfoText("$OSL_InfoMinLibidoPlayer")
        elseif(optionId == MinLibidoNPCOid)
            SetInfoText("$OSL_InfoMinLibidoNPC")
        elseif(optionId == BeingNudeBaselineOid)
            SetInfoText("$OSL_InfoNude")
        elseif(optionId == ViewingNudeBaselineOid)
            SetInfoText("$OSL_InfoViewingNude")
        elseif(optionId == EroticArmorBaselineOid)
            SetInfoText("$OSL_InfoErotic")
        elseif(optionId == SceneParticipantBaselineOid)
            SetInfoText("$OSL_InfoSceneParticipate")
        elseif(optionId == SceneViewerBaselineOid)
            SetInfoText("$OSL_InfoSceneView")
        elseif(optionId == VictimGainsArousalOid)
            SetInfoText("$OSL_InfoVictimGain")
        elseif(optionId == SceneBeginArousalOid)
            SetInfoText("$OSL_InfoSceneBegin")
        elseif(optionId == StageChangeArousalOid)
            SetInfoText("$OSL_InfoStageChange")
        elseif(optionId == OrgasmArousalLossOid)
            SetInfoText("$OSL_InfoOrgasmLoss")
        elseif(optionId == SceneEndArousalNoOrgasmOid)
            SetInfoText("$OSL_InfoSceneEndNoOrgasm")
        elseif(optionId == SceneEndArousalOrgasmOid)
            SetInfoText("$OSL_InfoSceneEndSLSO")
        elseif(optionId == ArousalRateOfChangeOid)
            SetInfoText("$OSL_InfoArousalRate")
        elseif(optionId == LibidoRateOfChangeOid)
            SetInfoText("$OSL_InfoLibidoRate")
        endif
    elseif(CurrentPage == "$OSL_System")
        if(optionId == DumpArousalData)
            SetInfoText("$OSL_InfoDumpData")
        elseif(optionId == ClearAllArousalData)
            SetInfoText("$OSL_InfoClearData")
        endif
    EndIf
endevent

event OnOptionMenuOpen(int optionId)
    if (CurrentPage == "$OSL_Keywords")
        if(optionId == ArmorListMenuOid)
            LoadArmorList()
        endif
    elseif (CurrentPage == "$OSL_Puppeteer")
        if(optionId == GenderPreferenceOid)
            SetMenuDialogStartIndex(Main.SlaFrameworkStub.GetGenderPreference(PuppetActor, true))
            SetMenuDialogDefaultIndex(3)
            SetMenuDialogOptions(GenderPreferenceList)
        endif
    elseif (CurrentPage == "$OSL_UI")
        if(optionId == ArousalBarDisplayModeOid)
            SetMenuDialogStartIndex(Main.ArousalBar.DisplayMode)
            SetMenuDialogDefaultIndex(1)
            SetMenuDialogOptions(ArousalBarDisplayModeNames)
        endif
    elseif (CurrentPage == "$OSL_Settings")
        if(optionId == DeviceBaselineGainTypeOid)
            LoadDeviceTypesList();
        endif
    endif
endevent

event OnOptionMenuAccept(int optionId, int index)
    If (CurrentPage == "$OSL_Keywords")
        If (optionId == ArmorListMenuOid)
            Form[] equippedArmor = OSLArousedNativeActor.GetAllEquippedArmor(Game.GetPlayer())
            SelectedArmor = equippedArmor[FoundArmorIds[index]] as Armor
            SetMenuOptionValue(optionId, FoundArmorNames[index])
            ArmorSelected()
        EndIf
    elseif (CurrentPage == "$OSL_Puppeteer")
        if(optionId == GenderPreferenceOid)
            Main.SlaFrameworkStub.SetGenderPreference(PuppetActor, index)
            SetMenuOptionValue(optionId, GenderPreferenceList[index])
        endif
    elseif (CurrentPage == "$OSL_UI")
        if(optionId == ArousalBarDisplayModeOid)
            Main.ArousalBar.SetDisplayMode(index)
            SetMenuOptionValue(optionId, ArousalBarDisplayModeNames[index])
        endif
    elseif (CurrentPage == "$OSL_Settings")
        if(optionId == DeviceBaselineGainTypeOid)
            SelectedDeviceTypeId = index
            SetMenuOptionValue(optionId, DeviceTypeNames[index])
            SetSliderOptionValue(DeviceBaselineGainValueOid, Main.DeviceBaselineModifications[index])
        endif
    endif
endevent

event OnOptionSliderOpen(int option)
    if(CurrentPage == "$OSL_Puppeteer")
        if(option == SetArousalOid)
            float arousal = 0
            arousal = OSLArousedNative.GetArousal(PuppetActor)
            SetSliderDialogStartValue(arousal)
            SetSliderDialogDefaultValue(0)
            SetSliderDialogRange(0, 100)
            SetSliderDialogInterval(1)
        elseif (option == SetLibidoOid)
            float libido = OSLArousedNative.GetLibido(PuppetActor)
            bool isPlayer = PuppetActor == Game.GetPlayer()
            SetSliderDialogStartValue(libido)
            if(isPlayer)
                SetSliderDialogDefaultValue(Main.MinLibidoValuePlayer)
                SetSliderDialogRange(Main.MinLibidoValuePlayer, 100)
            else
                SetSliderDialogDefaultValue(Main.MinLibidoValueNPC)
                SetSliderDialogRange(Main.MinLibidoValueNPC, 100)
            endif
            SetSliderDialogInterval(1)
        ElseIf (option == SetArousalMultiplierOid)
            float arousalMultiplier = OSLArousedNative.GetArousalMultiplier(PuppetActor)
            SetSliderDialogStartValue(arousalMultiplier)
            SetSliderDialogDefaultValue(kDefaultArousalMultiplier)
            SetSliderDialogRange(0, 10)
            SetSliderDialogInterval(0.2)
        endif
    ElseIf(CurrentPage == "$OSL_UI")
        if(option == ArousalBarXOid)
            SetSliderDialogStartValue(Main.ArousalBar.X)
            SetSliderDialogDefaultValue(980)
            SetSliderDialogRange(0, 1000)
        elseif(option == ArousalBarYOid)
            SetSliderDialogStartValue(Main.ArousalBar.Y)
            SetSliderDialogDefaultValue(160)
            SetSliderDialogRange(35, 710)
        endif
    elseIf (currentPage == "$OSL_Settings")
        if(option == MinLibidoPlayerOid)
            SetSliderDialogStartValue(Main.MinLibidoValuePlayer)
            SetSliderDialogDefaultValue(30)
            SetSliderDialogRange(0, 100)
        elseif(option == MinLibidoNPCOid)
            SetSliderDialogStartValue(Main.MinLibidoValueNPC)
            SetSliderDialogDefaultValue(80)
            SetSliderDialogRange(0, 100)
        elseif(option == SceneParticipantBaselineOid)
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
    if(currentPage == "$OSL_Puppeteer")
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
    elseif(currentPage == "$OSL_UI")
        if(option == ArousalBarXOid)
            Main.ArousalBar.SetPosX(value)
        elseif(option == ArousalBarYOid)
            Main.ArousalBar.SetPosY(value)
        endif
        SetSliderOptionValue(option, value)
    elseIf (currentPage == "$OSL_Settings")
        if(option == MinLibidoPlayerOid)
            Main.SetMinLibidoValue(true, value)
            SetSliderOptionValue(MinLibidoPlayerOid, value, "{1}")
        elseif(option == MinLibidoNPCOid)
            Main.SetMinLibidoValue(false, value)
            SetSliderOptionValue(MinLibidoNPCOid, value, "{1}")
        elseif(option == SceneParticipantBaselineOid)
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
            SetSliderOptionValue(SceneBeginArousalOid, value, "{1}")
        elseif(option == StageChangeArousalOid)
            Main.StageChangeArousalGain = value
            SetSliderOptionValue(StageChangeArousalOid, value, "{1}")
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
    if(currentPage == "$OSL_Puppeteer")
        if(option == SetArousalOid)
            OSLArousedNative.SetArousal(PuppetActor, 0)
            SetSliderOptionValue(SetArousalOid, 0, "{1}")
        elseif(option == SetLibidoOid)
            bool isPlayer = PuppetActor == Game.GetPlayer()
            if(isPlayer)
                OSLArousedNative.SetLibido(PuppetActor, Main.MinLibidoValuePlayer)
                SetSliderOptionValue(SetLibidoOid, Main.MinLibidoValuePlayer, "{1}")
            else
                OSLArousedNative.SetLibido(PuppetActor, Main.MinLibidoValueNPC)
                SetSliderOptionValue(SetLibidoOid, Main.MinLibidoValueNPC, "{1}")
            endif
        elseif(option == SetArousalMultiplierOid)
            OSLArousedNative.SetArousalMultiplier(PuppetActor, kDefaultArousalMultiplier)
            SetSliderOptionValue(SetArousalMultiplierOid, kDefaultArousalMultiplier, "{1}")
        elseif(option == GenderPreferenceOid)
            Main.SlaFrameworkStub.SetGenderPreference(PuppetActor, 3)
            SetMenuOptionValue(GenderPreferenceOid, GenderPreferenceList[3])
        endif
    elseif(currentPage == "$OSL_UI")
        if(option == ArousalBarXOid)
            Main.ArousalBar.SetPosX(980)
            SetSliderOptionValue(option, 980)
        elseif(option == ArousalBarYOid)
            Main.ArousalBar.SetPosY(160)
            SetSliderOptionValue(option, 160)
        endif
    elseif(CurrentPage == "$OSL_Settings")
        if(option == MinLibidoPlayerOid)
            Main.SetMinLibidoValue(true, 50)
            SetSliderOptionValue(MinLibidoPlayerOid, 50, "{1}")
        elseif(option == MinLibidoNPCOid)
            Main.SetMinLibidoValue(false, 50)
            SetSliderOptionValue(MinLibidoNPCOid, 50, "{1}")
        elseif(option == SceneParticipantBaselineOid)
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
            Main.SetArousalChangeRate(20)
            SetSliderOptionValue(ArousalRateOfChangeOid, 50, "{1}")
        elseif(option == LibidoRateOfChangeOid)
            Main.SetLibidoChangeRate(10)
            SetSliderOptionValue(LibidoRateOfChangeOid, 10, "{1}")
        endif
    endif
endevent

event OnOptionInputAccept(int option, string inputVal)
    if(option == RegisterKeywordOid)
        ; First check if this keyword can be found
        if(!Keyword.GetKeyword(inputVal))
            Debug.MessageBox("Keyword: " + inputVal + " not found. Please try again.")
            return
        endif

        if(OSLArousedNative.RegisterNewKeyword(inputVal))
            Debug.MessageBox("Keyword: " + inputVal + " Registered Successfully. Please save and load your game to see the new keyword in the list.")
        else
            Debug.MessageBox("Keyword: " + inputVal + " Failed to Register. Please try again or Check OSLAroused.log file.")
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
    Form[] equippedArmor = OSLArousedNativeActor.GetAllEquippedArmor(player)
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
    SetMenuDialogOptions(FoundArmorNames)
endfunction

function ArmorSelected()
    if(!SelectedArmor)
        return
    endif

    int index = 0
    while(index < RegisteredKeywords.Length)
        Keyword kw = RegisteredKeywords[index] as Keyword
        if(kw)
            RegisteredKeywordStates[index] = CheckKeyword(kw, RegisteredKeywordOids[index])
        endif
        index += 1
    endwhile
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

string[] function GetDeviceTypeNames()
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
    return DeviceTypeNames
endfunction

function LoadDeviceTypesList()
    SetMenuDialogOptions(GetDeviceTypeNames())
endfunction

function Log(string msg) global
    Debug.Trace("---OSLAroused--- [MCM] " + msg)
endfunction
