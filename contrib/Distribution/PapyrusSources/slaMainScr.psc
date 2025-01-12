Scriptname slaMainScr extends Quest  

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