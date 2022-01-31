scriptname OSLAroused_ModInterface hidden

function ModifyArousal(Actor target, float value) global
    Debug.Trace("---OSLAroused--- ModifyArousal: " + target.GetDisplayName() + "modified by val: " + value)
    OSLArousedNative.ModifyArousal(target, value)
endfunction

function ModifyArousalMultiple(Actor[] actorArray, float value) global
    Debug.Trace("---OSLAroused--- ModifyArousalMultiple: " + actorArray.Length + " actors modified by val: " + value)
    OSLArousedNative.ModifyArousalMultiple(actorArray, value)
endfunction
