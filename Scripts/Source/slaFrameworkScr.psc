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
endfunction

;Additive exposure
Int Function UpdateActorExposure(Actor act, Int modVal, String debugMsg = "")
    log("UpdateActorExposure: " + modVal + " msg: " + debugMsg)
    return OSLArousedNative.ModifyArousal(act, modVal) as Int
EndFunction

function OnActorArousalUpdated(Actor act, float newArousal)
    log("Updating SLAFramework Val: " + newArousal)
    ;Update Factions
    if(slaArousalFaction)
        act.SetFactionRank(slaArousalFaction, newArousal as int)
    endif
endfunction

Event ModifyExposure(Form actForm, float val)
    Log("ModifyExposure Event via Modevent for: " + actForm + " val: " + val)
    Actor akRef = actForm as Actor
    if(akRef)
        OSLArousedNative.ModifyArousal(akRef, val)
    endif
EndEvent


function Log(string msg) global
    Debug.Trace("----OSLAroused---- [slaFrameworkScr] - " + msg)
endfunction 