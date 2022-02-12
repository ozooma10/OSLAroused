Scriptname OSLAroused_MCM extends SKI_ConfigBase hidden

OSLAroused_Main Property Main Auto

OSLAroused_MCM Function Get() Global
	return Game.GetFormFromFile(0x806, "OSLAroused.esp") as OSLAroused_MCM
EndFunction

;---- Overview Properties ----
int ArousalStatusOid
int BaselineArousalStatusOid
int LibidoStatusOid

int SLAStubLoadedOid
int OArousedStubLoadedOid

;---- Settings ----
;Baseline
int EnableStatBuffsOid
int BeingNudeBaselineOid
int ViewingNudeBaselineOid
int SceneParticipantBaselineOid
int SceneViewerBaselineOid
int VictimGainsArousalOid

;Event-Based
int SceneBeginArousalOid
int StageChangeArousalOid

;---- Puppet Properties ----
Actor Property PuppetActor Auto
int Property SetArousalOid Auto
int Property SetLibidoOid Auto

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

int function GetVersion()
    return 200 ; 0.2.0
endfunction

Event OnConfigInit()
    ModName = "OSLAroused"

    Pages = new String[6]
    Pages[0] = "Overview"
    Pages[1] = "Puppeteer"
    Pages[2] = "Keywords"
    Pages[3] = "UI/Notifications"
    Pages[4] = "Settings"
    Pages[5] = "System"


    ArousalBarDisplayModeNames = new String[4]
    ArousalBarDisplayModeNames[0] = "Always Off"
    ArousalBarDisplayModeNames[1] = "Fade"
    ArousalBarDisplayModeNames[2] = "Toggle Showing"
    ArousalBarDisplayModeNames[3] = "Always On"
EndEvent

Event OnVersionUpdate(Int NewVersion)
    If (CurrentVersion != 0)
        OnConfigInit()
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

    ArousalStatusOid = AddTextOption("Arousal", OSLArousedNative.GetArousal(target))
    BaselineArousalStatusOid = AddTextOption("Baseline Arousal", OSLArousedNative.GetArousalBaseline(target))
    LibidoStatusOid = AddTextOption("Libido", OSLArousedNative.GetLibido(target))
endfunction

function PuppeteerPage(Actor target)
    if(target == none)
        AddHeaderOption("No Target Selected")
        return
    endif
    AddHeaderOption(target.GetLeveledActorBase().GetName())

    float arousal = OSLArousedNative.GetArousal(PuppetActor)
    SetArousalOid = AddSliderOption("Arousal", arousal, "{0}")

    float libido = OSLArousedNative.GetLibido(PuppetActor)
    SetLibidoOid = AddSliderOption("Libido", libido, "{0}")
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

    AddHeaderOption("Baseline Arousal Gains")
    SceneParticipantBaselineOid = AddSliderOption("Participating In Sex", Main.GetHourlySceneParticipantArousalModifier(), "{1}")
    VictimGainsArousalOid = AddToggleOption("Victim Gains Arousal", Main.VictimGainsArousal)
    SceneViewerBaselineOid = AddSliderOption("Spectating Sex", Main.GetHourlySceneViewerArousalModifier(), "{1}")
    BeingNudeBaselineOid = AddSliderOption("Being Nude", Main.GetEnableNudityIncreasesArousal(), "{1}")
    ViewingNudeBaselineOid = AddSliderOption("Viewing Nude", Main.GetHourlyNudityArousalModifier(), "{1}")

endfunction

function SettingsRightColumn()
    AddHeaderOption("Event-Based Arousal Gains")
    SceneBeginArousalOid = AddSliderOption("Sex Scene Begin", Main.SexlabStageChangeIncreasesArousal, "{1}")
    StageChangeArousalOid = AddSliderOption("Sex Stage Change", Main.SexlabStageChangeIncreasesArousal, "{1}")
endfunction

function SystemPage()
    AddHeaderOption("Native Data")
    DumpArousalData = AddTextOption("Dump Arousal Data", "RUN")
    ClearAllArousalData = AddTextOption("Clear All Arousal Data", "RUN")
    EnableDebugModeOid = AddToggleOption("Enable Debug Logging", Main.EnableDebugMode)
endfunction

event OnOptionSelect(int optionId)
    if(CurrentPage == "Settings")
        if(optionId == VictimGainsArousalOid)
            Main.VictimGainsArousal = !Main.VictimGainsArousal
            SetToggleOptionValue(VictimGainsArousalOid, Main.VictimGainsArousal)
        elseif(optionId == EnableStatBuffsOid)
            Main.SetArousalEffectsEnabled(!Main.EnableArousalStatBuffs) 
            SetToggleOptionValue(EnableStatBuffsOid, Main.EnableArousalStatBuffs)
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
        elseif(optionId == BeingNudeBaselineOid)
            SetInfoText("Amount Baseline Arousal is increased by when nude")
        elseif(optionId == ViewingNudeBaselineOid)
            SetInfoText("Amount Baseline Arousal is increased by when near naked NPCs")
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
    endif
endevent

event OnOptionMenuAccept(int optionId, int index)
    If (CurrentPage == "Keywords")
        If (optionId == ArmorListMenuOid)
            SelectedArmor = Game.GetPlayer().GetNthForm(FoundArmorIds[index]) as Armor
            SetMenuOptionValue(optionId, FoundArmorNames[index])
            ArmorSelected()
        EndIf
    elseif (CurrentPage == "UI/Notifications")
        if(optionId == ArousalBarDisplayModeOid)
            Main.ArousalBar.SetDisplayMode(index)
            SetMenuOptionValue(optionId, ArousalBarDisplayModeNames[index])
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
            SetSliderDialogStartValue(50)
            SetSliderDialogDefaultValue(50)
            SetSliderDialogRange(0, 100)
        elseif(option == SceneViewerBaselineOid)
            SetSliderDialogStartValue(20)
            SetSliderDialogDefaultValue(20)
            SetSliderDialogRange(0, 100)
        elseif(option == BeingNudeBaselineOid)
            SetSliderDialogStartValue(30)
            SetSliderDialogDefaultValue(30)
            SetSliderDialogRange(0, 100)
        elseif(option == ViewingNudeBaselineOid)
        elseif(option == SceneBeginArousalOid)
        elseif(option == StageChangeArousalOid)
        endif
    endif
endevent

event OnOptionSliderAccept(int option, float value)
    if(currentPage == "Puppeteer")
        if(option == SetArousalOid)
            OSLArousedNative.SetArousal(PuppetActor, value)
            SetSliderOptionValue(SetArousalOid, value, "{0}")
        elseif(option == SetLibidoOid)
            OSLArousedNative.SetLibido(PuppetActor, value)
            SetSliderOptionValue(SetLibidoOid, value, "{1}")
        endif
    elseif(currentPage == "UI/Notifications")
        if(option == ArousalBarXOid)
            Main.ArousalBar.SetPosX(value)
        elseif(option == ArousalBarYOid)
            Main.ArousalBar.SetPosY(value)
        endif
        SetSliderOptionValue(option, value)
    endif
endevent

event OnOptionDefault(int option)
    if(currentPage == "Puppeteer")
        if(option == SetArousalOid)
            OSLArousedNative.SetArousal(PuppetActor, 0)
            SetSliderOptionValue(SetArousalOid, 0, "{0}")
        elseif(option == SetLibidoOid)
            OSLArousedNative.SetLibido(PuppetActor, 0)
            SetSliderOptionValue(SetLibidoOid, 0, "{0}")
        endif
    elseif(currentPage == "UI")
        if(option == ArousalBarXOid)
            Main.ArousalBar.SetPosX(980)
            SetSliderOptionValue(option, 980)
        elseif(option == ArousalBarYOid)
            Main.ArousalBar.SetPosY(160)
            SetSliderOptionValue(option, 160)
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
