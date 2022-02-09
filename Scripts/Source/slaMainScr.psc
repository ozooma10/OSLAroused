Scriptname slaMainScr extends Quest  

bool Function IsActorNaked(Actor akRef)
    ;Naughty naughty, mods shouldnt call this
    return OSLArousedNative.IsActorNaked(akRef)
EndFunction