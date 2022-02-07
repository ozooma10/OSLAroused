
scriptname OSLAroused_ModInterface hidden

float function GetArousal(Actor target) global
    return OSLArousedNative.GetArousal(target)
    ;Log("GetArousal: " + target.GetDisplayName() + " val: " + arousal)
    ;return arousal
endfunction

float function GetArousalMultiplier(Actor target) global
    return OSLArousedNative.GetArousalMultiplier(target)
endfunction

float function GetExposure(Actor target) global
    return OSLArousedNative.GetExposure(target)
endfunction

float function ModifyArousal(Actor target, float value, string reason = "unspecified") global
    Log("ModifyArousal: " + target.GetDisplayName() + " modified by val: " + value + " Reason: " + reason)
    return OSLArousedNative.ModifyArousal(target, value)
endfunction

function ModifyArousalMultiple(Actor[] actorArray, float value, string reason = "unspecified") global
    Log("ModifyArousalMultiple: " + actorArray.Length + " actors modified by val: " + value + " Reason: " + reason)
    OSLArousedNative.ModifyArousalMultiple(actorArray, value)
endfunction

float function ModifyArousalMultiplier(Actor target, float value, string reason = "unspecified") global
    Log("ModifyArousalMultiplier: " + target.GetDisplayName() + " modified by val: " + value + " Reason: " + reason)
    return OSLArousedNative.ModifyArousalMultiplier(target, value)
endfunction

function RegisterOrgasm(Actor target) global
    Log("RegisterOrgasm: " + target.GetDisplayName())
    OSLArousedNative.RegisterActorOrgasm(target)
endfunction

function Log(string msg) global
    Debug.Trace("---OSLAroused--- [ModInterface] " + msg)
endfunction