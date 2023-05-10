Scriptname slaConfigScr extends Quest

Float Property DefaultExposureRate
    Float Function Get()
        Return 1.0
    EndFunction
EndProperty

;NOTE: Currently used in native dll but not configurable, so until then hardcode
Float Property TimeRateHalfLife = 2.0 Auto

;NOTE: Currently Not Used
Int Property SexOveruseEffect = 5 Auto hidden

Int Function GetVersion()
	return 28
EndFunction