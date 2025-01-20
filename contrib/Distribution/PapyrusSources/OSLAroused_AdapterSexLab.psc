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
    Log("OnAnimationStart")
    OSLArousedNative.RegisterSceneStart(false, tid, controller.Positions)

    ;OArousal mode sends a blast on scene start
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
    int exposureValue = 0
    Bool canHaveOrgasm = False

    if(!bInOslMode)
        exposureValue = ((controller.TotalTime / GetAnimationDuration(controller)) * -20) as Int
            
        If (controller.Animation.HasTag("Anal") || controller.Animation.HasTag("Vaginal") || controller.Animation.HasTag("Masturbation") || controller.Animation.HasTag("Fisting"))
            canHaveOrgasm = True
        EndIf
    endif

    
	

    int i = controller.Positions.Length
    while(i > 0)
        i -= 1
        actor act = controller.Positions[i]

        ;If in OSL Mode, Directly add arousal
        ;If in SLA Mode, Update last sex date so that arousal calculation is used
        if(bInOslMode)
            if(SLSODetected)
                if((controller.ActorAlias(act) as sslActorAlias).GetOrgasmCount() > 0)
                    OSLAroused_ModInterface.ModifyArousal(act, Main.SceneEndArousalOrgasmChange, "sexlab end - SLSO orgasm")
                elseif(Main.VictimGainsArousal || !controller.IsVictim(act))
                    OSLAroused_ModInterface.ModifyArousal(act, Main.SceneEndArousalNoOrgasmChange, "sexlab end - SLSO no orgasm")
                endif
            else
                OSLAroused_ModInterface.ModifyArousal(act, Main.SceneEndArousalNoOrgasmChange, "sexlab end - no orgasm (no SLSO)")
            endif
        else
            if(controller.IsVictim(act))
                OSLAroused_ModInterface.ModifyArousal(act, -10, "SLA Mode - Victim to SL Scene")
            endif

            int sex = act.GetLeveledActorBase().GetSex()
            ;If sex is male or unknown, dont need to check for belt
            if(sex == 0 || sex == -1)
                OSLAroused_ModInterface.ModifyArousal(act, exposureValue, "sexlab end - having orgasm (SLA Mode)")
                SLAModeUpdateActorOrgasmDate(act)
            else
                ;If female, check for belt
                bool bHasBelt = false
                if(DeviousBeltKeyword != none)
                    bHasBelt = act.WornHasKeyword(DeviousBeltKeyword)
                endif
                if(canHaveOrgasm && !bHasBelt)
                    OSLAroused_ModInterface.ModifyArousal(act, exposureValue, "sexlab end - having orgasm (SLA Mode)")
                    SLAModeUpdateActorOrgasmDate(act)
                else
                    Log("Actor: " + act.GetDisplayName() + " can not orgasm. has belt: " + bHasBelt)
                endif
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

Float Function GetAnimationDuration(sslThreadController bThread)
	If (bThread == None)
		return -1.0
	EndIf
	
	Float[] timeList =  bThread.Timers
	
	Float res = 0.0
	Float stageTimer = 0.0
	int i = 0
	int stageCount = bThread.animation.StageCount()
	
	While (i < timeList.length && i < stageCount)
	
		if i == stageCount - 1
			stageTimer = timeList[4]
		elseif i < 3
			stageTimer = timeList[i]
		else
			stageTimer = timeList[3]
		endIf
		
		res = res + stageTimer
		i += 1
	EndWhile
	
	return res
EndFunction


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

    Log("OnSexLabOrgasm: " + actorForm + " enjoyment: " + enjoyment)
    OSLArousedNative.RegisterActorOrgasm(act)

    ;Update arousal for any victims
    ;@TODO: Tie this into a lewdness system
    ; if(controller.Victims.Length > 0)
    ;     OSLAroused_ModInterface.ModifyArousalMultiple(controller.Victims, -20, "being sexlab victim")
    ; endif

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
