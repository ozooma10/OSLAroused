ScriptName OSLAroused_AdapterOStim Extends Quest Hidden

actor[] ActiveSceneActors

; OAroused Mode Vars
float[] previousModifiers
bool bEndOnDomOrgasm
bool bEndOnSubOrgasm

bool function LoadAdapter()
	;Looks like Ostims not Installed
    if (Game.GetModByName("Ostim.esp") == 255)
		return false
    endif
	
	OSexIntegrationMain OStim = OUtils.GetOStim()
	if (OStim == none || OStim.GetAPIVersion() < 23)
		debug.MessageBox("Your OStim version is out of date. OAroused requires a newer version.")
		return false
	endif

	RegisterForModEvent("ostim_orgasm", "OStimOrgasm")
	RegisterForModEvent("ostim_start", "OStimStart")
	RegisterForModEvent("ostim_end", "OStimEnd")
    return true
endfunction

Event OStimOrgasm(String EventName, String Args, Float Nothing, Form Sender)
	OSexIntegrationMain OStim = OUtils.GetOStim()

	actor orgasmer = OStim.GetMostRecentOrgasmedActor()
	OSLArousedNative.RegisterActorOrgasm(orgasmer)

	OSLAroused_Main main = OSLAroused_Main.Get()
	actor player = main.PlayerRef

	float reduceBy = (OStim.GetTimeSinceStart() / 120) * OStim.SexExcitementMult
    reduceBy = OSLArousedNative.ClampFloat(reduceBy, 0.75, 1.5)
    reduceBy = reduceBy * 55.0
    reduceBy = reduceBy + OSLArousedNative.GenerateRandomFloat(-5.0, 5.0)
    reduceBy = -reduceBy 

	OSLAroused_ModInterface.ModifyArousal(orgasmer, reduceBy, "ostim orgasm")

	CalculateStimMultipliers(OStim)

	if orgasmer == player
		if OSLAroused_ModInterface.GetArousal(player) < 15
			if bEndOnDomOrgasm
				OStim.EndOnDomOrgasm = true 
			endif 
			if bEndOnSubOrgasm
				OStim.EndOnSubOrgasm = true
			endif 
		endif 

		Main.ArousalBar.DisplayBarWithAutohide(10.0)
	endif 
EndEvent

Event OStimStart(String EventName, String Args, Float Nothing, Form Sender)
	Log("OStim Scene Start")
	OSLAroused_Main main = OSLAroused_Main.Get()
	OSexIntegrationMain OStim = OUtils.GetOStim()
	ActiveSceneActors = OStim.GetActors()
	previousModifiers = Utility.CreateFloatArray(3)

	CalculateStimMultipliers(OStim)

	OSLAroused_ModInterface.ModifyArousalMultiple(ActiveSceneActors, 5.0 * OStim.SexExcitementMult, "OStim Scene Start")

	Actor player = main.PlayerRef
	if (main.RequireLowArousalToEndScene && OStim.IsPlayerInvolved() && !OStim.HasSceneMetadata("SpecialEndConditions") && !(OStim.isvictim(player)))
		if player == OStim.GetDomActor()
			bEndOnDomOrgasm = OStim.EndOnDomOrgasm
			OStim.EndOnDomOrgasm = false 
		elseif player == OStim.GetSubActor()
			bEndOnSubOrgasm = OStim.EndOnSubOrgasm
			OStim.EndOnSubOrgasm = false 
		endif 
	endif

	OSLArousedNative.RegisterSceneStart(true, -1, ActiveSceneActors)
endevent

Event OStimEnd(String EventName, String Args, Float Nothing, Form Sender)
	Log("OStim Scene End")

	OSLAroused_Main main = OSLAroused_Main.Get()
	OSexIntegrationMain OStim = OUtils.GetOStim()

	; increase arousal for actors that did not orgasm
	int i = ActiveSceneActors.Length
	while i > 0
		i -= 1
		if OStim.GetTimesOrgasm(ActiveSceneActors[i]) < 1
			OSLAroused_ModInterface.ModifyArousal(ActiveSceneActors[i], 20.0, "OStim end - no orgasm")
		endif 
	endwhile
	
	if bEndOnDomOrgasm
		bEndOnDomOrgasm = false 
		OStim.EndOnDomOrgasm = true 
	endif 
	if bEndOnSubOrgasm
		bEndOnSubOrgasm = false 
		OStim.EndOnSubOrgasm = true 
	endif 

	OSLArousedNative.RemoveScene(true, -1)
endevent

Function CalculateStimMultipliers(OSexIntegrationMain OStim)
	int i = 0
	int max = ActiveSceneActors.Length
	while i < max 
		float arousal = OSLAroused_ModInterface.GetArousal(ActiveSceneActors[i])

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

		OStim.ModifyStimMult(ActiveSceneActors[i], modifyBy - previousModifiers[i])
		Log("Modding stim mult for: " + ActiveSceneActors[i] + ": " + (modifyBy - previousModifiers[i]))
		previousModifiers[i] = modifyBy

		i += 1
	endwhile
EndFunction

; ========== DEBUG RELATED ==================

function Log(string msg) global
    Debug.Trace("---OSLAroused--- [OStimAdapter] " + msg)
endfunction
