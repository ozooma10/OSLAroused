ScriptName OSLAroused_Main Extends Quest Hidden
; This Script is Based off OAroused by Sairion350
; All the good things are from Sairion and all the bad things are by me :) 

Actor Property PlayerRef Auto 

float Property ScanDistance = 5120.0 AutoReadOnly

keyword property EroticArmor auto

OSLAroused_ArousalBar Property ArousalBar Auto
OSLAroused_Conditions Property ConditionVars Auto 

bool property OStimAdapterLoaded = false Auto Hidden
OSLAroused_AdapterOStim Property OStimAdapter Auto

OSLAroused_AdapterDebug Property DebugAdapter Auto

int CheckArousalKey = 157
bool EnableNudityIncreasesArousal = true
bool Property EnableArousalStatBuffs = true Auto
float DefaultArousalMultiplier = 2.0

int property kArousalMode_SLAroused = 0 autoreadonly
int property kArousalMode_OAroused = 1 autoreadonly
int SelectedArousalMode


;OAroused Spells
Spell Property SLADesireSpell Auto

;SL Aroused Spells
Spell Property OArousedHornySpell Auto
Spell Property OArousedRelievedSpell Auto


;spell horny 
;spell relieved 

; ============== CORE LIFECYCLE ===================

Event OnInit()
	EroticArmor = Keyword.GetKeyword("EroticArmor")
	
	;horny.SetNthEffectArea

	;Initialize multiplier to 2 for player
	OSLArousedNative.SetArousalMultiplier(PlayerRef, 2.0)

	RegisterForModEvent("OSLA_PlayerArousalUpdated", "OnPlayerArousalUpdated")

	OnGameLoaded()

	Log("OSLAroused installed")
	Debug.Notification("OSLAroused installed")
EndEvent

Function OnGameLoaded()
	;horny = Game.GetFormFromFile(0x805, "oslaroused.esp") as spell
	;relieved = Game.GetFormFromFile(0x806, "oslaroused.esp") as spell

	OStimAdapterLoaded = OStimAdapter.LoadAdapter()
	Log("OStim Integration Status: " + OStimAdapterLoaded)
	DebugAdapter.LoadAdapter()

	RegisterForKey(CheckArousalKey)

	float arousal = OSLArousedNative.GetArousal(PlayerRef)
	ArousalBar.InitializeBar(arousal / 100)

	;Need to notify skse dll whether to check for player nudity
	OSLArousedNative.UpdatePlayerNudityCheck(EnableNudityIncreasesArousal)
EndFunction

event OnPlayerArousalUpdated(string eventName, string strVal, float newArousal, Form sender)
	ArousalBar.SetPercent(newArousal / 100.0)

	ConditionVars.OSLAroused_PlayerArousal = newArousal
	ConditionVars.OSLAroused_PlayerTimeRate = OSLArousedNative.GetTimeRate(PlayerRef)

	if EnableArousalStatBuffs
		ApplyArousedEffects(newArousal as int)
	else  
		RemoveAllArousalSpells()
	endif
endevent

; ========== AROUSAL EFFECTS ===========

Function ApplyArousedEffects(int arousal)
	if(SelectedArousalMode == kArousalMode_SLAroused)
		PlayerRef.RemoveSpell(SLADesireSpell)
		PlayerRef.AddSpell(SLADesireSpell, false)
		Debug.Trace("Enabled SLA Desire Spell")
	elseif(SelectedArousalMode == kArousalMode_OAroused)
		if arousal >= 40
			arousal -= 40
			float percent = arousal / 60.0
			ApplyHornySpell((percent * 25) as int)
		elseif arousal <= 10
			ApplyReliefSpell(10)
		else 
			RemoveAllArousalSpells()
		endif 
	endif
EndFunction

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
		ArousalBar.DisplayBarWithAutohide(10.0)
	endif 	
EndEvent


; ========= SETTINGS UPDATE =================
int function GetShowArousalKeybind()
	return CheckArousalKey
endfunction

float function GetDefaultArousalMultiplier()
	return DefaultArousalMultiplier
endfunction

bool function GetEnableNudityIncreasesArousal()
	return EnableNudityIncreasesArousal
endfunction

int function GetCurrentArousalMode()
	return SelectedArousalMode
endfunction

function SetCurrentArousalMode(int newMode)
	if(newMode < 0 || newMode > 1)
		return
	endif
	SelectedArousalMode = newMode
endfunction

function SetDefaultArousalMultiplier(float newVal)
	if(newVal < 0 || newVal > 10)
		return
	endif
	DefaultArousalMultiplier = newVal
	OSLArousedNative.UpdateDefaultArousalMultiplier(newVal)
endfunction

function SetPlayerNudityIncreasesArousal(bool newVal)
	EnableNudityIncreasesArousal = newVal
	OSLArousedNative.UpdatePlayerNudityCheck(newVal)
endfunction

function SetShowArousalKeybind(int newKey)
	UnregisterForKey(CheckArousalKey)
	CheckArousalKey = newKey
	RegisterForKey(newKey)
endfunction

; ========== DEBUG RELATED ==================

function Log(string msg) global
    Debug.Trace("---OSLAroused--- " + msg)
endfunction
