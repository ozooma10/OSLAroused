ScriptName OSLAroused_AdapterSexLab Extends Quest Hidden

bool function LoadAdapter()
	;Looks like Sexlab not Installed
    if (Game.GetModByName("SexLab.esm") == 255)
		return false
    endif

	RegisterForModEvent("HookAnimationStart", "OnAnimationStart")
	RegisterForModEvent("HookAnimationEnd", "OnAnimationEnd")
	RegisterForModEvent("HookStageStart", "OnStageStart")
	RegisterForModEvent("SexLabOrgasm", "OnSexLabOrgasm")
    return true
endfunction

event OnAnimationStart(int tid, bool hasPlayer)
    Log("OnAnimationStart")
    SexLabFramework sexlab = SexLabUtil.GetAPI() 
    if(!sexlab)
        return
    endif
    sslThreadController controller = sexlab.GetController(tid)
    OSLArousedNative.RegisterSceneStart(false, tid, controller.Positions)

	OSLAroused_Main main = OSLAroused_Main.Get()
    ;OArousal mode sends a blast on scene start
    if(Main.GetCurrentArousalMode() == Main.kArousalMode_OAroused)
        OSLAroused_ModInterface.ModifyArousalMultiple(controller.Positions, 5.0, "Sexlab Animation Start")
    endif
endevent

event OnAnimationEnd(int tid, bool hasPlayer)
    OSLArousedNative.RemoveScene(false, tid)

	OSLAroused_Main main = OSLAroused_Main.Get()
    if(main.GetCurrentArousalMode() == main.kArousalMode_OAroused)
        OArousedModeAnimationEnd(tid)
    endif
endevent

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

    OSLArousedNative.RegisterActorOrgasm(act)

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


function OArousedModeAnimationEnd(int tid)
    SexLabFramework sexlab = SexLabUtil.GetAPI() 
    if(!sexlab)
        return
    endif
    ;increase arousal for actors who did not org
    sslThreadController controller = sexlab.GetController(tid)
    int i = controller.ActorAlias.Length
    while(i > 0)
        i -= 1
        sslActorAlias slot = controller.ActorAlias[i]
        If (slot && slot.GetOrgasmCount() < 1)
            OSLAroused_ModInterface.ModifyArousal(slot.ActorRef, 20.0, "sexlab end - no orgasm")
        EndIf
    endwhile
endfunction

; ========== DEBUG RELATED ==================

function Log(string msg) global
    Debug.Trace("---OSLAroused--- [SexlabAdapter] " + msg)
endfunction
