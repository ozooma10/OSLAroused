ScriptName OSLAroused_AdapterSexLab Extends Quest Hidden

OSLAroused_Main Main

bool SLSODetected = false
Keyword DeviousBeltKeyword = None

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

    DeviousBeltKeyword = Game.GetFormFromFile(0x3330, "Devious Devices - Assets.esm") as Keyword

    SLSODetected = Game.GetModByName("SLSO.esp") != 255
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
    OSLArousedNative.RegisterSceneStart(false, tid, controller.Positions)

    OSLAroused_ModInterface.ModifyArousalMultiple(controller.Positions, Main.SceneBeginArousalGain, "Sexlab Animation Start")
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

    bool bInOslMode = OSLArousedNativeConfig.IsInOSLMode()
    int i = controller.Positions.Length
    while(i > 0)
        i -= 1
        actor act = controller.Positions[i]

        if(SLSODetected)
            if((controller.ActorAlias(act) as sslActorAlias).GetOrgasmCount() > 0)
                OSLAroused_ModInterface.ModifyArousal(act, Main.SceneEndArousalOrgasmChange, "sexlab end - SLSO orgasm")
            elseif(Main.VictimGainsArousal || !controller.IsVictim(act))
                OSLAroused_ModInterface.ModifyArousal(act, Main.SceneEndArousalNoOrgasmChange, "sexlab end - SLSO no orgasm")
            endif
        else
            ;TODO: Try and improve vanilla SL orgasm detection. 
            ; Currently Check if this actor orgasmed by comparing the actors last sex time and their last orgasm time, should be a relatively short amount of game time between them
            ; May get false negatives if actor orgasmed early in scene
            ; 0.03 game time is ~2 minute real time
            bool bDidOrgasm = OSLArousedNative.GetDaysSinceLastOrgasm(act) < 0.03

            if(bDidOrgasm)
                OSLAroused_ModInterface.ModifyArousal(act, Main.SceneEndArousalOrgasmChange, "sexlab end - did orgasm ")
            elseif(Main.VictimGainsArousal || !controller.IsVictim(act))
                OSLAroused_ModInterface.ModifyArousal(act, Main.SceneEndArousalNoOrgasmChange, "sexlab end - no orgasm")
            endif
        endif
    endwhile
endevent

function SLAModeUpdateActorOrgasmDate(Actor akRef)
    if(akRef == none)
        return
    endif
    OSLAroused_ModInterface.RegisterOrgasm(akRef)
    ;Update Timerate (which is libido in sla mode)
    OSLAroused_ModInterface.ModifyLibido(akRef, Main.SLAOveruseEffect, "SLA Mode Orgasm (OveruseEffect)")
endfunction

Event OnStageStart(int tid, bool HasPlayer)
    If (Main.StageChangeArousalGain == 0.0)
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
        OSLAroused_ModInterface.ModifyArousalMultiple(actors, Main.StageChangeArousalGain, "sexlab scene change")
    else
        int i = actors.Length
        while(i > 0)
            i -= 1
            If (!controller.IsVictim(actors[i]))
                OSLAroused_ModInterface.ModifyArousal(actors[i], Main.StageChangeArousalGain, "sexlab scene change")
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

    if(OSLArousedNativeConfig.IsInOSLMode())
        OSLArousedNative.RegisterActorOrgasm(act)
    else
        SLAModeUpdateActorOrgasmDate(act)
    endif


    ;Lower arousal on orgasm
    ;@TODO: Improve this function
    int exposureMod = Main.OrgasmArousalChange as int
    ;@TODO: Check for belt
    OSLAroused_ModInterface.ModifyArousal(act, exposureMod, "sexlab orgasm")
EndEvent

; ========== SCENE RELATED ==================
function StartMasturbationScene(Actor target)
    sslBaseAnimation[] animations
    Actor[] actors = new Actor[1]
    actors[0] = target

    SexLabFramework SexLab = SexLabUtil.GetAPI() 
    If 0 == target.GetLeveledActorBase().GetSex()
        animations = SexLab.GetAnimationsByTag(1, "Masturbation", "M")
    Else
        animations = SexLab.GetAnimationsByTag(1, "Masturbation", "F")
    EndIf

    Int id = SexLab.StartSex(actors, animations)
    If id < 0
        Debug.Notification("SexLab animation failed to start [" + id + "]")
    EndIf
endfunction

; ========== DEBUG RELATED ==================

function Log(string msg) global
    Debug.Trace("---OSLAroused--- [SexlabAdapter] " + msg)
endfunction
