ScriptName OSLAroused_AdapterToys Extends Quest Hidden

bool function LoadAdapter()
	;Looks like toys not Installed
    if (Game.GetModByName("Toys.esm") == 255)
		return false
    endif

	RegisterForModEvent("ToysClimax", "ToysClimax")
	RegisterForModEvent("ToysStartLove", "ToysStartLove")
	RegisterForModEvent("ToysLoveSceneEnd", "ToysLoveSceneEnd")
    return true
endfunction

Event ToysClimax(String EventName, String SceneName, Float Nothing, Form Sender)
    ;Log("Climax: " + EventName + "  --  " + SceneName + "  --  " + Nothing)
EndEvent

Event ToysStartLove(String EventName, String LoveName, Float Nothing, Form Sender)
    Log("Start: " + EventName + "  --  " + LoveName + "  --  " + Nothing)

    ;OSLAroused_ModInterface.ModifyArousalMultiple()    
EndEvent

Event ToysLoveSceneEnd(String EventName, String LoveName, Float Nothing, Form Sender)
    Log("End: " + EventName + "  --  " + LoveName + "  --  " + Nothing)
EndEvent

; ========== DEBUG RELATED ==================

function Log(string msg) global
    Debug.Trace("---OSLAroused--- [ToysAdapter] " + msg)
endfunction
