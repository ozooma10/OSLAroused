Scriptname slaConfigScr extends Quest

Float Property DefaultExposureRate
    Float Function Get()
        Return OSLAroused_Main.Get().GetDefaultArousalMultiplier()
    EndFunction
EndProperty
