ScriptName OSLAroused_Main Extends Quest Hidden
; This Script is Based off OAroused by Sairion350
; All the good things are from Sairion and all the bad things are by me :) 

Actor Property PlayerRef Auto 

float Property ScanDistance = 5120.0 AutoReadOnly

keyword property EroticArmor auto

OSLAroused_ArousalBar Property ArousalBar Auto

bool property OStimAdapterLoaded = false Auto Hidden
OSLAroused_AdapterOStim Property OStimAdapter Auto

OSLAroused_AdapterDebug Property DebugAdapter Auto

int CheckArousalKey = 157
bool EnableNudityIncreasesArousal = true
bool Property EnableArousalStatBuffs = true Auto

;spell horny 
;spell relieved 

; ============== CORE LIFECYCLE ===================

Event OnInit()
	EroticArmor = Keyword.GetKeyword("EroticArmor")
	
	;Initialize multiplier to 1 for player
	OSLArousedNative.SetArousalMultiplier(PlayerRef, 1.0)

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

Event OnUpdate()
	Log("Main Update OnUpdate")
	if(OStimAdapterLoaded)
		OStimAdapter.UpdateAdapter()
	endif
EndEvent

event OnPlayerArousalUpdated(string eventName, string strVal, float newArousal, Form sender)
	ArousalBar.SetPercent(newArousal / 100.0)

	if EnableArousalStatBuffs
		ApplyArousedEffects(newArousal as int)
	else  
		RemoveAllArousalSpells()
	endif
endevent

; ========== AROUSAL EFFECTS ===========

Function ApplyArousedEffects(int arousal)
	if arousal >= 40
		arousal -= 40
		float percent = arousal / 60.0
		ApplyHornySpell((percent * 25) as int)
	elseif arousal <= 10
		ApplyReliefSpell(10)
	else 
		RemoveAllArousalSpells()
	endif 
	
EndFunction

Function ApplyHornySpell(int magnitude)
	;horny.SetNthEffectMagnitude(0, magnitude)
	;horny.SetNthEffectMagnitude(1, magnitude)

	;playerref.RemoveSpell(relieved)
	;playerref.RemoveSpell(horny)
	;playerref.AddSpell(horny, false)
EndFunction

Function ApplyReliefSpell(int magnitude)
	;relieved.SetNthEffectMagnitude(0, magnitude)
	;relieved.SetNthEffectMagnitude(1, magnitude)

	;playerref.RemoveSpell(horny)
	;playerref.RemoveSpell(relieved)
	;playerref.AddSpell(relieved, false)
EndFunction

Function RemoveAllArousalSpells()
	;playerref.RemoveSpell(horny)
	;playerref.RemoveSpell(relieved)
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

bool function GetEnableNudityIncreasesArousal()
	return EnableNudityIncreasesArousal
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
