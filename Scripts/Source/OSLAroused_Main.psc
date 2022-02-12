ScriptName OSLAroused_Main Extends Quest Hidden
; This Script is Based off OAroused by Sairion350
; All the good things are from Sairion and all the bad things are by me :) 

OSLAroused_Main Function Get() Global
	return Game.GetFormFromFile(0x806, "OSLAroused.esp") as OSLAroused_Main
EndFunction

Actor Property PlayerRef Auto 

slaFrameworkScr SlaFrameworkStub

float Property ScanDistance = 5120.0 AutoReadOnly

OSLAroused_ArousalBar Property ArousalBar Auto
OSLAroused_Conditions Property ConditionVars Auto 

bool property OStimAdapterLoaded = false Auto Hidden
OSLAroused_AdapterOStim Property OStimAdapter Auto

bool property SexLabAdapterLoaded = false Auto Hidden
OSLAroused_AdapterSexLab Property SexLabAdapter Auto

bool property InvalidSlaFound = false Auto Hidden
bool property SlaStubLoaded = false Auto Hidden
bool property InvalidOArousedFound = false Auto Hidden
bool property OArousedStubLoaded = false Auto Hidden

; ============ SETTINGS ============
int CheckArousalKey = 157
int ToggleArousalBarKey = 157

;Do not directly set these settings. Use the associated Set function (so that dll is updated)
float Property SceneParticipationBaselineIncrease = 20.0 Auto
float Property SceneViewingBaselineIncrease = 20.0 Auto
bool Property VictimGainsArousal = false Auto
float Property NudityBaselineIncrease = 20.0 Auto
float Property ViewingNudityBaselineIncrease = 20.0 Auto

float Property SceneBeginArousalGain = 10.0 Auto
float Property StageChangeArousalGain = 3.0 Auto

bool Property EnableArousalStatBuffs = true Auto

bool Property EnableDebugMode = true Auto

; OStim Specific
bool Property RequireLowArousalToEndScene Auto

; ============ SPELLS =============
;OAroused Spells
Spell Property SLADesireSpell Auto

;SL Aroused Spells
Spell Property OArousedHornySpell Auto
Spell Property OArousedRelievedSpell Auto

; ============== CORE LIFECYCLE ===================

Event OnInit()
	;Initialize multiplier to 2 for player
	;OSLArousedNative.SetArousalMultiplier(PlayerRef, DefaultArousalMultiplier)
	;OSLArousedNative.SetArousal(PlayerRef, 5)

	OnGameLoaded()

	Log("OSLAroused installed")
	Debug.Notification("OSLAroused installed")
EndEvent

Function OnGameLoaded()
	RegisterForModEvent("OSLA_ActorArousalUpdated", "OnActorArousalUpdated")
	RegisterForModEvent("OSLA_ActorNakedUpdated", "OnActorNakedUpdated")

	SlaStubLoaded = false
	OArousedStubLoaded = false
	InvalidSlaFound = false
	InvalidOArousedFound = false
	SlaFrameworkStub = none
    if (Game.GetModByName("SexLabAroused.esm") != 255)
		SlaFrameworkStub = Game.GetFormFromFile(0x4290F, "SexLabAroused.esm") as slaFrameworkScr
		if(SlaFrameworkStub && SlaFrameworkStub.IsOSLArousedStub)
			SlaStubLoaded = true
			SlaFrameworkStub.OnGameLoaded()
		else
			SlaFrameworkStub = none
			InvalidSlaFound = true
			Debug.MessageBox("[OSL Aroused]: Non-OSLAroused SexLabAroused.esm Detected. Do not Install any version of SexLab Aroused, And ensure OSLAroused overwrites esm and slaFrameworkScr.psc")
		endif
	endif
	if (Game.GetModByName("OAroused.esp") != 255)
		If (OArousedScript.GetOAroused().IsOSLArousedStub)
			OArousedStubLoaded = true
		else
			InvalidOArousedFound = true
			Debug.MessageBox("[OSL Aroused]: Non-OSLAroused OAroused.esp Detected. Do not Install any version of OAroused, And ensure OAroused overwrites esp and OArousedScript.psc")
		EndIf
	endif

	OStimAdapterLoaded = OStimAdapter.LoadAdapter()
	Log("OStim Integration Status: " + OStimAdapterLoaded)

	SexLabAdapterLoaded = SexLabAdapter.LoadAdapter()
	Log("SexLab IntegrationStatus: " + SexLabAdapterLoaded)

	OSLAroused_MCM.Get().OnGameLoaded()

	RegisterForKey(CheckArousalKey)
	RegisterForKey(ToggleArousalBarKey)

	; Bootstrap settings
	; Need to notify skse dll whether to check for player nudity
	OSLArousedNativeConfig.SetSceneParticipantBaseline(SceneParticipationBaselineIncrease)
	OSLArousedNativeConfig.SetSceneViewingBaseline(SceneViewingBaselineIncrease)
	OSLArousedNativeConfig.SetSceneVictimGainsArousal(VictimGainsArousal)
	OSLArousedNativeConfig.SetBeingNudeBaseline(NudityBaselineIncrease)
	OSLArousedNativeConfig.SetViewingNudeBaseline(ViewingNudityBaselineIncrease)

	RemoveAllArousalSpells()
	if(EnableArousalStatBuffs)
		ApplyArousedEffects()
	endif

	float arousal = OSLArousedNative.GetArousal(PlayerRef)
	ArousalBar.InitializeBar(arousal / 100)

	;Initial Player Naked State
	if(OSLArousedNative.IsActorNaked(PlayerRef))
		OnActorNakedUpdated("", "", 1, PlayerRef)
	else
		OnActorNakedUpdated("", "", 0, PlayerRef)
	endif
EndFunction

;@TODO: This causes Error: Incorrect number of arguments passed. Expected 1, got 4. to throw in papyrus log.
;Works as expected need to debug through papyrus
event OnActorArousalUpdated(string eventName, string strArg, float newExposure, Form sender)
	Actor act = sender as Actor

	;Need to get amount of arousal to add on top of exposure 
	float lastOrgasmArousal = OSLArousedNative.GetLastOrgasmFrustrationArousal(act)
	float newArousal = newExposure + lastOrgasmArousal

	;Log("OnActorArousalUpdated for: " + act.GetDisplayName() + " Exposure: " + newExposure + " Frustration: " + lastOrgasmArousal + " Arousal: " + newArousal)
	if(act == PlayerRef)
		ArousalBar.SetPercent(newArousal / 100.0)

		ConditionVars.OSLAroused_PlayerArousal = newArousal
		ConditionVars.OSLAroused_PlayerTimeRate = OSLArousedNative.GetTimeRate(PlayerRef)

		if EnableArousalStatBuffs
			; We check for OArousedMode so we can bypass an arousal fetch and directly use updated val
			ApplyArousedEffects()
		else  
			RemoveAllArousalSpells()
		endif

	endif

	if(SlaFrameworkStub)
		SlaFrameworkStub.OnActorArousalUpdated(act, newArousal, newExposure)
	endif
endevent

event OnActorNakedUpdated(string eventName, string strArg, float actorNakedFloat, Form sender)
	bool isActorNaked = actorNakedFloat > 0
	Actor act = sender as Actor
	Log("OnActorNakedUpdated for: " + act.GetDisplayName() + " - newNaked: " + isActorNaked)
	
	if(SlaFrameworkStub && act)
		SlaFrameworkStub.OnActorNakedUpdated(act, isActorNaked)
	endif
endevent


; ========== AROUSAL EFFECTS ===========
function SetArousalEffectsEnabled(bool enabled)
	EnableArousalStatBuffs = enabled
	if EnableArousalStatBuffs
		ApplyArousedEffects()
	else  
		RemoveAllArousalSpells()
	endif
endfunction

Function ApplyArousedEffects()
	PlayerRef.RemoveSpell(SLADesireSpell)
	PlayerRef.AddSpell(SLADesireSpell, false)
EndFunction

Function ApplyOArousedEffects(int arousal)
	if arousal >= 40
		arousal -= 40
		float percent = arousal / 60.0
		ApplyHornySpell((percent * 25) as int)
	elseif arousal <= 10
		ApplyReliefSpell(10)
	else 
		RemoveAllArousalSpells()
	endif 
endfunction

Function ApplyHornySpell(int magnitude)
	OArousedHornySpell.SetNthEffectMagnitude(0, magnitude)
	OArousedHornySpell.SetNthEffectMagnitude(1, magnitude)

	playerref.RemoveSpell(OArousedRelievedSpell)
	playerref.RemoveSpell(OArousedHornySpell)
	playerref.AddSpell(OArousedHornySpell, false)
EndFunction

Function ApplyReliefSpell(int magnitude)
	OArousedRelievedSpell.SetNthEffectMagnitude(0, magnitude)
	OArousedRelievedSpell.SetNthEffectMagnitude(1, magnitude)

	playerref.RemoveSpell(OArousedHornySpell)
	playerref.RemoveSpell(OArousedRelievedSpell)
	playerref.AddSpell(OArousedRelievedSpell, false)
EndFunction

Function RemoveAllArousalSpells()
	playerref.RemoveSpell(SLADesireSpell)
	playerref.RemoveSpell(OArousedHornySpell)
	playerref.RemoveSpell(OArousedRelievedSpell)
EndFunction

Event OnKeyDown(int keyCode)
	if Utility.IsInMenuMode()
		return 
	endif
	if keyCode == CheckArousalKey
		
		Debug.Notification(PlayerRef.GetDisplayName() + " arousal level " + OSLArousedNative.GetArousal(PlayerRef))
		if(ArousalBar.DisplayMode == ArousalBar.kDisplayMode_Fade)
			ArousalBar.UpdateDisplay()
		endif
		Actor crosshairTarget = Game.GetCurrentCrosshairRef() as Actor
		If (crosshairTarget != none)
			Debug.Notification(crosshairTarget.GetDisplayName() + " arousal level " + OSLArousedNative.GetArousal(crosshairTarget))
			OSLAroused_MCM.Get().PuppetActor = crosshairTarget
		Else
			OSLAroused_MCM.Get().PuppetActor = PlayerRef
		EndIf
	endif
	If (keyCode == ToggleArousalBarKey && ArousalBar.DisplayMode == ArousalBar.kDisplayMode_Toggle)
		ArousalBar.UpdateDisplay()
	EndIf
EndEvent


; ========= SETTINGS UPDATE =================
int function GetShowArousalKeybind()
	return CheckArousalKey
endfunction

int function GetToggleArousalBarKeybind()
	return ToggleArousalBarKey
endfunction

function SetSceneParticipantBaseline(float newVal)
	SceneParticipationBaselineIncrease = newVal
	OSLArousedNativeConfig.SetSceneParticipantBaseline(newVal)
endfunction

function SetSceneViewingBaseline(float newVal)
	SceneViewingBaselineIncrease = newVal
	OSLArousedNativeConfig.SetSceneViewingBaseline(newVal)
endfunction

function SetSceneVictimGainsArousal(bool newVal)
	VictimGainsArousal = newVal
	OSLArousedNativeConfig.SetSceneVictimGainsArousal(newVal)
endfunction

function SetBeingNudeBaseline(float newVal)
	NudityBaselineIncrease = newVal
	OSLArousedNativeConfig.SetBeingNudeBaseline(newVal)
endfunction

function SetViewingNudeBaseline(float newVal)
	ViewingNudityBaselineIncrease = newVal
	OSLArousedNativeConfig.SetViewingNudeBaseline(newVal)
endfunction

function SetShowArousalKeybind(int newKey)
	UnregisterForKey(CheckArousalKey)
	CheckArousalKey = newKey
	RegisterForKey(newKey)
endfunction

function SetToggleArousalBarKeybind(int newKey)
	UnregisterForKey(ToggleArousalBarKey)
	ToggleArousalBarKey = newKey
	RegisterForKey(newKey)
endfunction

; ========== DEBUG RELATED ==================

function Log(string msg)
	If (EnableDebugMode)
		Debug.Trace("---OSLAroused--- " + msg)
	EndIf
endfunction
