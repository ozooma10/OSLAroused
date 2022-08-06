Scriptname slaFrameworkScr extends Quest
{This is a stubbed version of slaFrameworkScr that redirects external mod requests to use OSLAroused}

Faction Property slaArousal Auto
Faction Property slaExposure Auto

slaMainScr Property slaMain Auto
slaConfigScr Property slaConfig Auto

Faction slaArousalFaction
Faction slaExposureFaction
Faction slaNakedFaction
Faction slaGenderPreference

bool Property IsOSLArousedStub = true Auto

Int Property slaArousalCap = 100 AutoReadOnly

function OnGameLoaded()
    slaMain = Game.GetFormFromFile(0x42D62, "SexLabAroused.esm") as slaMainScr
    slaConfig = Game.GetFormFromFile(0x1C6E0, "SexLabAroused.esm") as slaConfigScr

    slaArousalFaction = Game.GetFormFromFile(0x3FC36, "SexLabAroused.esm") as Faction
    slaArousal = slaArousalFaction
    slaExposureFaction = Game.GetFormFromFile(0x25837, "SexLabAroused.esm") as Faction
    slaExposure = slaExposureFaction
    slaNakedFaction = Game.GetFormFromFile(0x77F87, "SexLabAroused.esm") as Faction

    slaGenderPreference = Game.GetFormFromFile(0x79A72, "SexLabAroused.esm") as Faction

    RegisterForModEvent("slaUpdateExposure", "ModifyExposure")
	RegisterForSingleUpdate(120)
endfunction

Int Function GetVersion()
	Return 20140124
EndFunction

int Function GetActorArousal(Actor akRef)
    if(akRef == none || akRef.IsChild())
        return -2
    endif

    return OSLAroused_ModInterface.GetArousal(akRef) as int
EndFunction

int Function GetActorExposure(Actor akRef)
    if(akRef == none)
        return -2
    endif

    return OSLAroused_ModInterface.GetExposure(akRef) as int
EndFunction

float Function GetActorExposureRate(Actor akRef)
    if(akRef == None)
        return -2
    endif

    return OSLAroused_ModInterface.GetArousalMultiplier(akRef)
EndFunction

Float Function GetActorTimeRate(Actor akRef)
    if(akRef == none)
        return -2.0
    endif    
    ;NOTE: TimeRate no longer Relevant in OSL Aroused. ArousalChangeRate is similarish but different values so cant be directly referenced
    return 10.0
EndFunction

Float Function SetActorTimeRate(Actor akRef, Float val)
    if(akRef == none)
        return -2.0
    endif
    ;NOTE: TimeRate no longer Relevant in OSL Aroused. ArousalChangeRate is similarish but different values so cant be directly referenced
    return 10.0 
EndFunction

Float Function UpdateActorTimeRate(Actor akRef, Float val)
    if(akRef == none)
        return -2.0
    endif
    ;NOTE: TimeRate no longer Relevant in OSL Aroused. ArousalChangeRate is similarish but different values so cant be directly referenced
    return 10.0 
EndFunction

;Additive exposure
Int Function UpdateActorExposure(Actor act, Int modVal, String debugMsg = "")
    return OSLAroused_ModInterface.ModifyArousal(act, modVal, "slaframework UpdateActorExposure") as Int
EndFunction

Int Function SetActorExposure(Actor akRef, Int val)
    if(akRef == none)
        return -2
    endif
    return OSLAroused_ModInterface.SetArousal(akRef, val) as int
EndFunction

Float Function SetActorExposureRate(Actor akRef, Float val)
    if(akRef == none)
        return -2.0
    endif
    return OSLAroused_ModInterface.SetArousalMultiplier(akRef, val, "slaframework SetActorExposureRate")
EndFunction


;Additive exposure rate
float function UpdateActorExposureRate(Actor akRef, float val)
    If (akRef == none)
        return -2
    EndIf

    return OSLAroused_ModInterface.ModifyArousalMultiplier(akRef, val, "slaframework UpdateActorExposureRate")
endfunction

function OnActorArousalUpdated(Actor act, float newArousal, float newExposure)
    ;Update Factions
    if(slaArousalFaction)
        act.SetFactionRank(slaArousalFaction, newArousal as int)
    endif
    if(slaExposureFaction)
        act.SetFactionRank(slaExposureFaction, newExposure as int)
    endif
endfunction

function OnActorNakedUpdated(Actor act, bool newNaked)
    if(slaNakedFaction)
        if(newNaked)
            act.SetFactionRank(slaNakedFaction, 0)
        else
            act.SetFactionRank(slaNakedFaction, -2)
        endif
    endif
endfunction

Event ModifyExposure(Form actForm, float val)
    Actor akRef = actForm as Actor
    if(akRef)
        Log("ModifyExposure Event via Modevent for: " + akRef.GetDisplayName() + " val: " + val)
        OSLAroused_ModInterface.ModifyArousal(akRef, val, "slaframework ModifyExposure")
    endif
EndEvent

Function UpdateActorOrgasmDate(Actor akRef)
    if(akRef == none)
        return
    endif
    OSLAroused_ModInterface.RegisterOrgasm(akRef)
EndFunction

Float Function GetActorDaysSinceLastOrgasm(Actor akRef)
    return OSLAroused_ModInterface.GetActorDaysSinceLastOrgasm(akRef)
EndFunction

;@TODO: Dont use Orgasm :(
Int Function GetActorHoursSinceLastSex(Actor akRef)
	If (akRef == None)
		return -2
	EndIf
	
    return (OSLAroused_ModInterface.GetActorDaysSinceLastOrgasm(akRef) * 24) as Int
EndFunction

;Send an updatecomplete event every 120 seconds
;Since OSLAroused update cycle occurs outside of Papyrus and not "Heartbeat" based like in sla, nothing really to bind to
Event OnUpdate()
    log("OnUpdate")
	RegisterForSingleUpdate(120) ;Another update in two more minutes
    SendModEvent("sla_UpdateComplete")
EndEvent


;==== NOT IMPLEMENTED
bool Function IsActorArousalLocked(Actor akRef)
    return false
EndFunction

bool Function IsActorArousalBlocked(Actor akRef)
    return false
EndFunction

Function SetActorExhibitionist(Actor akRef, bool val = false)
    return
endfunction
bool Function IsActorExhibitionist(Actor akRef)
    return false
endfunction

; 0 - Male
; 1 - Female
; 2 - Both
; 3 - SexLab
Int Function GetGenderPreference(Actor akRef, Bool forConfig = False)
	If (akRef == None)
		return -2
	EndIf
			
	int res = akRef.GetFactionRank(slaGenderPreference)
	If (res < 0 || res == 3)
		If (forConfig == True)
			Return 3
		EndIf
	
        if (Game.GetModByName("SexLab.esm") == 255)
            return -1
        endif

        SexLabFramework sexlab = SexLabUtil.GetAPI() 
		;;;Credits to Doombell for this piece of code
		int ratio = sexlab.Stats.GetSexuality(akRef)
		if ratio > 65
			res =  (-(akRef.GetLeveledActorBase().GetSex() - 1))
		ElseIf ratio < 35
			res =  akRef.GetLeveledActorBase().GetSex()
		Else
			res =  2
		EndIf
	EndIf
	Return res
EndFunction

Function SetGenderPreference(Actor akRef, Int gender)
    If (akRef == None)
		return
	EndIf
	
	akRef.SetFactionRank(slaGenderPreference, gender)
endfunction 

function Log(string msg) global
    Debug.Trace("----OSLAroused---- [slaFrameworkScr] - " + msg)
endfunction 