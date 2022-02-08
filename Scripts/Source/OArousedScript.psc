ScriptName OArousedScript Extends Quest
{This is a stubbed version of OAroused main script that redirects external mod requests to use OSLAroused}

float Property ScanDistance = 5120.0 AutoReadOnly

bool Property IsOSLArousedStub = true Auto

oarousedscript Function GetOAroused() Global
	return game.GetFormFromFile(0x806, "OSLAroused.esp") as OArousedScript
EndFunction

Keyword Property EroticArmor
    Keyword Function Get()
        Return OSLAroused_MCM.Get().EroticArmorKeyword
    EndFunction
EndProperty

float Function GetArousal(Actor act)
    return OSLArousedNative.GetArousal(act)
EndFunction

float Function ModifyArousal(Actor act, float by)
    return OSLAroused_ModInterface.ModifyArousal(act, by, "oaroused modify")
EndFunction

float Function SetArousal(Actor act, float value, bool updateAccessTime = true)
    return OSLArousedNative.SetArousal(act, value)
EndFunction