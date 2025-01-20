Scriptname slaConfigScr extends Quest

Float Property DefaultExposureRate
    Float Function Get()
        Return OSLAroused_Main.Get().SLADefaultExposureRate
    EndFunction
EndProperty


Float Property TimeRateHalfLife
    Float Function Get()
        Return OSLAroused_Main.Get().SLATimeRateHalfLife
    EndFunction
EndProperty

;NOTE: Currently Not Used
Int Property SexOveruseEffect
    Int Function Get()
        Return OSLAroused_Main.Get().SLAOveruseEffect
    EndFunction
EndProperty

Int Function GetVersion()
	return 28
EndFunction