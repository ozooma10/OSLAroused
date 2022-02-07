Scriptname slaFrameworkScr extends Quest
{This is a stubbed version of slaFrameworkScr that redirects external mod requests to use OSLAroused}

Faction slaArousalFaction
Faction slaExposureFaction
Faction slaNakedFaction

function OnGameLoaded()
    slaArousalFaction = Game.GetFormFromFile(0x3FC36, "SexLabAroused.esm") as Faction
    slaExposureFaction = Game.GetFormFromFile(0x25837, "SexLabAroused.esm") as Faction
    slaNakedFaction = Game.GetFormFromFile(0x77F87, "SexLabAroused.esm") as Faction

    RegisterForModEvent("slaUpdateExposure", "ModifyExposure")
	RegisterForSingleUpdate(120)
endfunction

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

;Additive exposure
Int Function UpdateActorExposure(Actor act, Int modVal, String debugMsg = "")
    return OSLAroused_ModInterface.ModifyArousal(act, modVal, "slaframework UpdateActorExposure") as Int
EndFunction

;Additive exposure rate
float function UpdateActorExposureRate(Actor akRef, float val)
    If (akRef == none)
        return -2
    EndIf

    return OSLAroused_ModInterface.ModifyArousalMultiplier(akRef, val, "slaframework UpdateActorExposureRate")
endfunction

function OnActorArousalUpdated(Actor act, float newArousal)
    ;Update Factions
    if(slaArousalFaction)
        act.SetFactionRank(slaArousalFaction, newArousal as int)
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
    Log("ModifyExposure Event via Modevent for: " + actForm + " val: " + val)
    Actor akRef = actForm as Actor
    if(akRef)
        OSLAroused_ModInterface.ModifyArousal(akRef, val, "slaframework ModifyExposure")
    endif
EndEvent

Function UpdateActorOrgasmDate(Actor akRef)
    if(akRef == none)
        return
    endif
    OSLAroused_ModInterface.RegisterOrgasm(akRef)
EndFunction


;Send an updatecomplete event every 120 seconds
;Since OSLAroused update cycle occurs outside of Papyrus and not "Heartbeat" based like in sla, nothing really to bind to
Event OnUpdate()
    log("OnUpdate")
	RegisterForSingleUpdate(120) ;Another update in two more minutes
    SendModEvent("sla_UpdateComplete")
EndEvent

function Log(string msg) global
    Debug.Trace("----OSLAroused---- [slaFrameworkScr] - " + msg)
endfunction 