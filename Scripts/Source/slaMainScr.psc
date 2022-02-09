Scriptname slaMainScr extends Quest  

bool Function IsActorNaked(Actor akRef)
    ;Naughty naughty, mods shouldnt call this
    return OSLArousedNative.IsActorNaked(akRef)
EndFunction


;@TODO: Hook this into the native dll to return nearby actors
Actor [] function getLoadedActors(int lockNum)
	Actor[] emptyArray
	return emptyArray
endFunction

;This isnt necessary
bool function UnlockScan(int lockNum)
    return true
endfunction