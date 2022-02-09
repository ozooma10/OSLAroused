Scriptname slaFrameworkScr extends Quest
{This is a stubbed version of slaFrameworkScr that redirects external mod requests to use OSLAroused}

Faction Property slaArousal Auto
Faction Property slaExposure Auto

slaConfigScr Property slaConfig Auto

Faction slaArousalFaction
Faction slaExposureFaction
Faction slaNakedFaction

bool Property IsOSLArousedStub = true Auto

function OnGameLoaded()
    slaConfig = Game.GetFormFromFile(0x4290F, "SexLabAroused.esm") as slaConfigScr

    slaArousalFaction = Game.GetFormFromFile(0x3FC36, "SexLabAroused.esm") as Faction
    slaArousal = slaArousalFaction
    slaExposureFaction = Game.GetFormFromFile(0x25837, "SexLabAroused.esm") as Faction
    slaExposure = slaExposureFaction
    slaNakedFaction = Game.GetFormFromFile(0x77F87, "SexLabAroused.esm") as Faction

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
    return OSLAroused_ModInterface.GetActorTimeRate(akRef)
EndFunction

Float Function UpdateActorTimeRate(Actor akRef, Float val)
    if(akRef == none)
        return -2.0
    endif
    return OSLAroused_ModInterface.ModifyActorTimeRate(akRef, val)
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

function Log(string msg) global
    Debug.Trace("----OSLAroused---- [slaFrameworkScr] - " + msg)
endfunction 