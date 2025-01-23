Scriptname slaMainScr extends Quest  

slaConfigScr Property slaConfig Auto

function OnGameLoaded()
    slaConfig = Game.GetFormFromFile(0x1C6E0, "SexLabAroused.esm") as slaConfigScr

EndFunction

bool Function IsActorNaked(Actor akRef)
    ;Naughty naughty, mods shouldnt call this
    return OSLArousedNative.IsActorNaked(akRef)
EndFunction


Actor [] function getLoadedActors(int lockNum)
    return OSLArousedNative.GetLastScannedActors();
endFunction

;This isnt necessary
bool function UnlockScan(int lockNum)
    return true
endfunction

; ==== NO-OP STUBBED FUNCTIONS

Float Property updateFrequency = 120.00 Auto hidden 

int function IsAnimatingFemales()
    return 0
endfunction

function setUpdateFrequency(Float newFreq)
    updateFrequency = newFreq
endfunction

Function UpdateDesireSpell()
    OSLAroused_Main.Get().SetArousalEffectsEnabled(slaConfig.IsDesireSpell)
EndFunction