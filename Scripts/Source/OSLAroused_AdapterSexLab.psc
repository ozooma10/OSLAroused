ScriptName OSLAroused_AdapterSexLab Extends Quest Hidden

OSLAroused_Main Main

bool function LoadAdapter()
	;Looks like Sexlab not Installed
    if (Game.GetModByName("SexLab.esm") == 255)
		return false
    endif

    Main = OSLAroused_Main.Get()

	RegisterForModEvent("HookAnimationStart", "OnAnimationStart")
	RegisterForModEvent("HookAnimationEnd", "OnAnimationEnd")
	RegisterForModEvent("HookStageStart", "OnStageStart")
	RegisterForModEvent("SexLabOrgasm", "OnSexLabOrgasm")
    return true
endfunction

event OnAnimationStart(int tid, bool hasPlayer)
    SexLabFramework sexlab = SexLabUtil.GetAPI() 
    if(!sexlab)
        return
    endif
    sslThreadController controller = sexlab.GetController(tid)
    ;If this event came from ostimlab (ie its a OStim scene, then dont process)
    If (controller.HasTag("OStimLab"))
        return
    EndIf
    Log("OnAnimationStart")
    OSLArousedNative.RegisterSceneStart(false, tid, controller.Positions)

    ;OArousal mode sends a blast on scene start
    OSLAroused_ModInterface.ModifyArousalMultiple(controller.Positions, 5.0, "Sexlab Animation Start")
endevent

event OnAnimationEnd(int tid, bool hasPlayer)
    OSLArousedNative.RemoveScene(false, tid)

    SexLabFramework sexlab = SexLabUtil.GetAPI() 
    if(!sexlab)
        return
    endif
    ;increase arousal for actors who did not org
    sslThreadController controller = sexlab.GetController(tid)

    ;If this event came from ostimlab (ie its a OStim scene, then dont process)
    If (controller.HasTag("OStimLab"))
        return
    EndIf

    int i = controller.Positions.Length
    while(i > 0)
        i -= 1
        actor act = controller.Positions[i]
        If ((controller.ActorAlias(act) as sslActorAlias).GetOrgasmCount() < 1 && (!controller.IsVictim(act) || Main.VictimGainsArousal))
            OSLAroused_ModInterface.ModifyArousal(act, 20.0, "sexlab end - no orgasm")
        EndIf
    endwhile
endevent

Event OnStageStart(int tid, bool HasPlayer)
    If (!Main.SexlabStageChangeIncreasesArousal)
        return
    EndIf

    SexLabFramework sexlab = SexLabUtil.GetAPI() 
    if(!sexlab)
        return
    endif

    sslThreadController controller = sexlab.GetController(tid)

    ;If this event came from ostimlab (ie its a OStim scene, then dont process)
    If (controller.HasTag("OStimLab"))
        return
    EndIf

    Actor[] actors = controller.Positions
    if(actors.length < 1)
        return
    endif

    if(Main.VictimGainsArousal || controller.Victims.Length == 0)
        OSLAroused_ModInterface.ModifyArousalMultiple(actors, Main.SexlabStageChangeArousalGain, "sexlab scene change")
    else
        int i = actors.Length
        while(i > 0)
            i -= 1
            If (!controller.IsVictim(actors[i]))
                OSLAroused_ModInterface.ModifyArousal(actors[i], Main.SexlabStageChangeArousalGain, "sexlab scene change")
            EndIf
        endwhile
    endif
endevent

Event OnSexLabOrgasm(Form actorForm, int enjoyment, int orgasmCount)
    Actor act = actorForm as Actor
    SexLabFramework sexlab = SexLabUtil.GetAPI() 
    if(!act || !sexlab)
        return
    endif
    sslThreadController controller = sexlab.GetActorController(act)
    if(!controller)
        return
    endif

    ;If this event came from ostimlab (ie its a OStim scene, then dont process)
    If (controller.HasTag("OStimLab"))
        return
    EndIf

    Log("OnSexLabOrgasm: " + actorForm + " enjoyment: " + enjoyment)
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

; ========== DEBUG RELATED ==================

function Log(string msg) global
    Debug.Trace("---OSLAroused--- [SexlabAdapter] " + msg)
endfunction
