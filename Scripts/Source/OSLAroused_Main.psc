ScriptName OSLAroused_Main Extends Quest Hidden
; This Script is Based off OAroused by Sairion350
; All the good things are from Sairion and all the bad things are by me :) 

Actor Property PlayerRef Auto 

string LastCheckTimeStorageKey = "OSLA_LastTime"
string ArousalStorageKey = "OSLA_Arousal"
string ArousalMultiplierStorageKey = "OSLA_Multiplier"

float Property ScanDistance = 5120.0 AutoReadOnly

keyword property EroticArmor auto

OSLAroused_ArousalBar Property ArousalBar Auto

bool property OStimAdapterLoaded = false Auto Hidden
OSLAroused_AdapterOStim Property OStimAdapter Auto

OSLAroused_AdapterDebug Property DebugAdapter Auto

int Property CheckArousalKey = 157 Auto
bool Property EnableNudityIncreasesArousal = true Auto
bool Property EnableArousalStatBuffs = true Auto

;spell horny 
;spell relieved 

; ============== PUBLICish API ===================

float function GetArousal(actor target)
	float lastCheckTime = StorageUtil.GetFloatValue(target, LastCheckTimeStorageKey)
	float curTime = Utility.GetCurrentGameTime()
	float timePassed = curtime - lastCheckTime
	
	StorageUtil.SetFloatValue(target, LastCheckTimeStorageKey, curTime); Save last check time to NPC

	float newArousal
	if (lastCheckTime <= 0.0) || ((timePassed) > 3.0) ;never calculated, or very old data
		newArousal = PO3_SKSEFunctions.GenerateRandomFloat(0.0, 75.0)
		
		if (lastCheckTime <= 0.0)
			float randomArousal = PO3_SKSEFunctions.GenerateRandomFloat(0.75, 1.25)
			StorageUtil.SetFloatValue(target, ArousalMultiplierStorageKey, PO3_SKSEFunctions.GenerateRandomFloat(0.75, 1.25))
		endif 
	else 
		float currentVal = StorageUtil.GetFloatValue(target, ArousalStorageKey)
		float arousalMultiplier = StorageUtil.GetFloatValue(target, ArousalMultiplierStorageKey)
		newArousal = currentVal + ((timePassed * 25.0) * arousalMultiplier)
	endif 
	
	return SetArousal(target, newArousal, false)
EndFunction

float Function SetArousal(actor npc, float value, bool updateAccessTime = true)
	if updateAccessTime
		StorageUtil.SetFloatValue(npc, LastCheckTimeStorageKey, utility.GetCurrentGameTime())
	endif 

	value = papyrusutil.ClampFloat(value, 0.0, 100.0)
	
	StorageUtil.SetFloatValue(npc, ArousalStorageKey, value)

	if npc == playerref
		ArousalBar.SetPercent(value / 100.0)

		if EnableArousalStatBuffs
			ApplyArousedEffects(value as int)
		else  
			RemoveAllArousalSpells()
		endif
	endif 

	return value
EndFunction

float Function ModifyArousal(actor npc, float by)
	Log("Modify Arousal Multiplier: " + StorageUtil.GetFloatValue(npc, ArousalMultiplierStorageKey))

	if by > 0.0 
		by *= StorageUtil.GetFloatValue(npc, ArousalMultiplierStorageKey)
	endif 

	return SetArousal(npc, GetArousal(npc) + by, false)
EndFunction

Function ModifyArousalMultiple(actor[] acts, float amount)
	{increase arousal by the amount}
	int i = 0 
	int max = acts.Length
	while i < max 
		ModifyArousal(acts[i], (amount))
		i += 1
	EndWhile
endfunction

; ============== CORE LIFECYCLE ===================

Event OnInit()
	EroticArmor = Keyword.GetKeyword("EroticArmor")
	
	;Initialize multiplier to 1 for player
	StorageUtil.SetFloatValue(PlayerRef, ArousalMultiplierStorageKey, 1.0)

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

	float arousal = GetArousal(PlayerRef)
	ArousalBar.InitializeBar(arousal / 100)
EndFunction

Event OnUpdate()
	Log("Main Update OnUpdate")
	if(OStimAdapterLoaded)
		OStimAdapter.UpdateAdapter()
	endif
EndEvent

Event OnUpdateGameTime()
	GetArousal(PlayerRef)

	RegisterForSingleUpdateGameTime(6)	
EndEvent

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

; ========== DEBUG RELATED ==================

function Log(string msg) global
    Debug.Trace("---OSLAroused--- " + msg)
endfunction
