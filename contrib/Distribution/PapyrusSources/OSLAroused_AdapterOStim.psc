;Standalone Support Greatly based off the patch from SimonPhil
ScriptName OSLAroused_AdapterOStim Extends Quest Hidden

actor[] ActiveSceneActors ;Still needed for legacy ostim

float[] previousModifiersThread0
float[] previousModifiersThread1
float[] previousModifiersThread2
float[] previousModifiersThread3
float[] previousModifiersThread4
float[] previousModifiersThread5
float[] previousModifiersThread6
float[] previousModifiersThread7
float[] previousModifiersThread8
float[] previousModifiersThread9

int function LoadAdapter()
	;Looks like Ostims not Installed
    if (Game.GetModByName("Ostim.esp") == 255)
		return 0
    endif
	
	OSexIntegrationMain OStim = OUtils.GetOStim()
	if (OStim == none || OStim.GetAPIVersion() < 23)
		debug.MessageBox("Your OStim version is out of date. OAroused requires a newer version.")
		return 0
	endif

	RegisterForModEvent("ostim_orgasm", "OStimOrgasm")
	RegisterForModEvent("ostim_start", "OStimStart")
	RegisterForModEvent("ostim_end", "OStimEnd")

	RegisterForModEvent("ostim_actor_orgasm", "OStimOrgasmThread")
	RegisterForModEvent("ostim_thread_start", "OStimStartThread")
	RegisterForModEvent("ostim_thread_end", "OStimEndThread")
	if (OStim.GetAPIVersion() >= 29)
		return 1
	endif
	return 2
EndFunction

Event OStimStart(String EventName, String Args, Float Nothing, Form Sender)
	OSexIntegrationMain OStim = OUtils.GetOStim()
	; If this is OStim NG, bail out (Since Below code is processed in OStimStartThread)
	if (OStim.GetAPIVersion() >= 29)
		return
	endif

	ActiveSceneActors = OUtils.GetOStim().GetActors()
	HandleStartScene(0, ActiveSceneActors)
EndEvent

Event OStimStartThread(String EventName, String Args, float ThreadID, Form Sender)
	HandleStartScene(ThreadID as int, OThread.GetActors(ThreadID as int))
EndEvent

Event OStimEnd(String EventName, String Args, Float Nothing, Form Sender)
	; If this is OStim NG, bail out (Since Below code is processed in OStimEndThread)
	if (OUtils.GetOStim().GetAPIVersion() >= 29)
		return
	endif
	HandleEndScene(0, ActiveSceneActors)
EndEvent

Event OStimEndThread(String EventName, String Args, Float ThreadID, Form Sender)
	HandleEndScene(ThreadID as int, OThread.GetActors(ThreadID as int))
EndEvent

Event OStimOrgasm(String EventName, String Args, Float Nothing, Form Sender)
	OSexIntegrationMain OStim = OUtils.GetOStim()
	; If this is OStim NG, bail out (Since Below code is processed in OStimEndThread)
	if (OStim.GetAPIVersion() >= 29)
		return
	endif

	actor orgasmer = OStim.GetMostRecentOrgasmedActor()
	if orgasmer
		HandleActorOrgasm(0, orgasmer)
	endif
EndEvent

Event OStimOrgasmThread(String EventName, String Args, Float ThreadID, Form Sender)
	if sender as Actor
		HandleActorOrgasm(0, sender as Actor)
	endif
EndEvent

; ========== SHARED HANDLERS ================

Function HandleStartScene(int threadId, Actor[] threadActors)
	Log("OStim Scene Started in Thread: " + threadId)
	CreatePreviousModifiers(ThreadID)
	CalculateStimMultipliers(ThreadID, threadActors)

	OSLAroused_ModInterface.ModifyArousalMultiple(threadActors, OSLAroused_Main.Get().SceneBeginArousalGain, "OStim Scene Start")
	OSLArousedNative.RegisterSceneStart(true, threadId, threadActors)
EndFunction

Function HandleEndScene(int threadId, Actor[] threadActors)
	Log("OStim Scene Ended in Thread: " + threadId)
	OSLAroused_Main main = OSLAroused_Main.Get()
	OSexIntegrationMain OStim = OUtils.GetOStim()
	; increase arousal for actors that did not orgasm
	int i = threadActors.Length
	while i > 0
		i -= 1
		if OStim.GetTimesOrgasm(threadActors[i]) < 1
			OSLAroused_ModInterface.ModifyArousal(threadActors[i], main.SceneEndArousalNoOrgasmChange, "OStim end - no orgasm")
		else
			OSLAroused_ModInterface.ModifyArousal(threadActors[i], main.SceneEndArousalOrgasmChange, "OStim end - orgasm")
		endif 
	endwhile
	OSLArousedNative.RemoveScene(true, threadId)
EndFunction

Function HandleActorOrgasm(int threadId, Actor targetActor)
	Log("OStim Actor Orgasm in Thread: " + threadId + " For: " + targetActor)
	OSLArousedNative.RegisterActorOrgasm(targetActor)

	OSLAroused_Main Main = OSLAroused_Main.Get()
	OSLAroused_ModInterface.ModifyArousal(targetActor, Main.OrgasmArousalChange, "ostim orgasm")

	if targetActor == Main.PlayerRef
		if(Main.ArousalBar.DisplayMode == Main.ArousalBar.kDisplayMode_Fade)
			Main.ArousalBar.UpdateDisplay()
		endif
	endif
EndFunction

; ========== THREAD HANDLING ================

Function CalculateStimMultipliers(int threadId, Actor[] threadActors)
	float[]  Modifiers = GetPreviousModifiers(threadId)
	int i = 0
	int max = threadActors.Length
	while i < max 
		float arousal = OSLAroused_ModInterface.GetArousal(threadActors[i])
		float modifyBy
		if arousal >= 95
			modifyBy = 1.25
		elseif arousal <= 5
			modifyBy = -0.35
		elseif arousal <= 40 
			modifyBy = 0.0
		else 
			arousal -= 40.0
			modifyBy = (arousal/100.0)
		endif 

		OUtils.GetOStim().ModifyStimMult(threadActors[i], modifyBy - Modifiers[i])
		ModifyPreviousModifiers(threadId, i, modifyBy)
		i += 1
	endwhile
EndFunction

Function CreatePreviousModifiers(int ThreadID)
	if ThreadID == 0
		previousModifiersThread0 = Utility.CreateFloatArray(10)
	elseif ThreadID == 1
		previousModifiersThread1 = Utility.CreateFloatArray(10)
	elseif ThreadID == 2
		previousModifiersThread2 = Utility.CreateFloatArray(10)
	elseif ThreadID == 3
		previousModifiersThread3 = Utility.CreateFloatArray(10)
	elseif ThreadID == 4
		previousModifiersThread4 = Utility.CreateFloatArray(10)
	elseif ThreadID == 5
		previousModifiersThread5 = Utility.CreateFloatArray(10)
	elseif ThreadID == 6
		previousModifiersThread6 = Utility.CreateFloatArray(10)
	elseif ThreadID == 7
		previousModifiersThread7 = Utility.CreateFloatArray(10)
	elseif ThreadID == 8
		previousModifiersThread8 = Utility.CreateFloatArray(10)
	elseif ThreadID == 9
		previousModifiersThread9 = Utility.CreateFloatArray(10)
	endif
EndFunction

Function ModifyPreviousModifiers(int ThreadID, int index, float modify)
	if ThreadID == 0
		previousModifiersThread0[index] = modify
	elseif ThreadID == 1
		previousModifiersThread1[index] = modify
	elseif ThreadID == 2
		previousModifiersThread2[index] = modify
	elseif ThreadID == 3
		previousModifiersThread3[index] = modify
	elseif ThreadID == 4
		previousModifiersThread4[index] = modify
	elseif ThreadID == 5
		previousModifiersThread5[index] = modify
	elseif ThreadID == 6
		previousModifiersThread6[index] = modify
	elseif ThreadID == 7
		previousModifiersThread7[index] = modify
	elseif ThreadID == 8
		previousModifiersThread8[index] = modify
	elseif ThreadID == 9
		previousModifiersThread9[index] = modify
	endif
EndFunction

float[] Function GetPreviousModifiers(int ThreadID)
	if ThreadID == 0
		return previousModifiersThread0
	elseif ThreadID == 1
		return previousModifiersThread1
	elseif ThreadID == 2
		return previousModifiersThread2
	elseif ThreadID == 3
		return previousModifiersThread3
	elseif ThreadID == 4
		return previousModifiersThread4
	elseif ThreadID == 5
		return previousModifiersThread5
	elseif ThreadID == 6
		return previousModifiersThread6
	elseif ThreadID == 7
		return previousModifiersThread7
	elseif ThreadID == 8
		return previousModifiersThread8
	elseif ThreadID == 9
		return previousModifiersThread9
	endif
	return Utility.CreateFloatArray(10)
EndFunction

; ========== DEBUG RELATED ==================

function Log(string msg) global
    Debug.Trace("---OSLAroused--- [OStimAdapter] " + msg)
endfunction
