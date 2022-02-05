ScriptName OSLAroused_AdapterSexLab Extends Quest Hidden

bool function LoadAdapter()
	;Looks like Sexlab not Installed
    if (Game.GetModByName("SexLab.esm") == 255)
		return false
    endif

	RegisterForModEvent("HookStageStart", "OnStageStart")
	RegisterForModEvent("SexLabOrgasm", "OnSexLabOrgasm")
    return true
endfunction

Event OnStageStart(int tid, bool HasPlayer)
    Log("OnStageStart")
    SexLabFramework sexlab = SexLabUtil.GetAPI() 
    if(!sexlab)
        return
    endif

    sslThreadController controller = sexlab.GetController(tid)
    Actor[] actors = controller.Positions
    if(actors.length < 1)
        return
    endif
    
    if(controller.Animation.HasTag("Foreplay"))
        OSLAroused_ModInterface.ModifyArousalMultiple(actors, 1, "sexlab foreplay")
    endif

    ;@TODO: Notify skse of scene change to propgate arousal to nearby npcs
endevent

Event OnSexLabOrgasm(Form actorForm, int enjoyment, int orgasmCount)
    Log("OnSexLabOrgasm: " + actorForm + " enjoyment: " + enjoyment)
    Actor act = actorForm as Actor
    SexLabFramework sexlab = SexLabUtil.GetAPI() 
    if(!act || !sexlab)
        return
    endif
    sslThreadController controller = sexlab.GetActorController(act)
    if(!controller)
        return
    endif

    ;Update arousal for any victims
    ;@TODO: Tie this into a lewdness system
    if(controller.Victims.Length > 0)
        OSLAroused_ModInterface.ModifyArousalMultiple(controller.Victims, -10, "being sexlab victim")
    endif

    ;Lower arousal on orgasm
    ;@TODO: Improve this function
    int exposureMod = ((controller.TotalTime / controller.GetAnimationRunTime()) * -20.0) as int
    ;@TODO: Check for belt
    OSLAroused_ModInterface.ModifyArousal(act, exposureMod, "sexlab orgasm")
EndEvent


; ========== DEBUG RELATED ==================

function Log(string msg) global
    Debug.Trace("---OSLAroused--- [SexlabAdapter] " + msg)
endfunction
