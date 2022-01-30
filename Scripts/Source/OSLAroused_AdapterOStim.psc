ScriptName OSLAroused_AdapterOStim Extends Quest Hidden

OSLAroused_Main Property Main Auto
Actor Property PlayerRef Auto

bool Property RequireLowArousalToEndScene Auto

actor[] ActiveSceneActors

function UpdateAdapter()
	; Do not process if adapter not loaded
	if(!Main.OStimAdapterLoaded)
		return
	endif

	OSexIntegrationMain OStim = OUtils.GetOStim()
    if(OStim && OStim.AnimationRunning())
		if(OStim.GetODatabase().IsSexAnimation(OStim.GetCurrentAnimationOID()))
			OSLArousedNative.ModifyArousalMultiple(ActiveSceneActors, 1.5 * OStim.SexExcitementMult)

			actor[] nearby = MiscUtil.ScanCellNPCs(ActiveSceneActors[0], Main.ScanDistance)
			float closeEnough  = Main.ScanDistance / 8

			int i = 0
			int max = nearby.Length
			while i < max
				if (!ActiveSceneActors[0].IsDetectedBy(nearby[i]) && (ActiveSceneActors[0].GetDistance(nearby[i]) > closeEnough)) || OStim.IsActorInvolved(nearby[i])
					nearby[i] = none
				endif 

				i += 1
			EndWhile

			OSLArousedNative.ModifyArousalMultiple(PapyrusUtil.RemoveActor(nearby, none), 5.0 * OStim.SexExcitementMult)
		endif
        RegisterForSingleUpdate(15.0)
	endif
endfunction

Event OnUpdate()
    UpdateAdapter()
endevent

Event OStimOrgasm(String EventName, String Args, Float Nothing, Form Sender)
	Log("OStim OStimOrgasm")
	OSexIntegrationMain OStim = OUtils.GetOStim()
	actor orgasmer = OStim.GetMostRecentOrgasmedActor()

	float reduceBy = (OStim.GetTimeSinceStart() / 120) * OStim.SexExcitementMult
    reduceBy = papyrusutil.ClampFloat(reduceBy, 0.75, 1.5)
    reduceBy = reduceBy * 55.0
    reduceBy = reduceBy + PO3_SKSEFunctions.GenerateRandomFloat(-5.0, 5.0)
    reduceBy = -reduceBy 

	OSLArousedNative.ModifyArousal(orgasmer, reduceBy)

	CalculateStimMultipliers()

	if orgasmer == playerref
		if OSLArousedNative.GetArousal(playerref) < 15
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

bool bEndOnDomOrgasm
bool bEndOnSubOrgasm
Event OStimStart(String EventName, String Args, Float Nothing, Form Sender)
	Log("OStim Scene Start")
	OSexIntegrationMain OStim = OUtils.GetOStim()
	ActiveSceneActors = OStim.GetActors()

	previousModifiers = PapyrusUtil.FloatArray(3)
	CalculateStimMultipliers()

	OSLArousedNative.ModifyArousalMultiple(ActiveSceneActors, 5.0 * OStim.SexExcitementMult)

	if (RequireLowArousalToEndScene && OStim.IsPlayerInvolved() && !OStim.HasSceneMetadata("SpecialEndConditions") && !(OStim.isvictim(PlayerRef)))
		if PlayerRef == OStim.GetDomActor()
			bEndOnDomOrgasm = OStim.EndOnDomOrgasm
			OStim.EndOnDomOrgasm = false 
		elseif PlayerRef == OStim.GetSubActor()
			bEndOnSubOrgasm = OStim.EndOnSubOrgasm
			OStim.EndOnSubOrgasm = false 
		endif 
	endif 

	RegisterForSingleUpdate(1.0)
endevent

Event OStimEnd(String EventName, String Args, Float Nothing, Form Sender)
	Log("OStim Scene End")
	OSexIntegrationMain OStim = OUtils.GetOStim()
	; increase arousal for actors that did not orgasm
	int i = 0 
	int max = ActiveSceneActors.Length
	while i < max 
		if OStim.GetTimesOrgasm(ActiveSceneActors[i]) < 1
			OSLArousedNative.ModifyArousal(ActiveSceneActors[i], 20.0)
		endif 

		i += 1
	endwhile

	if bEndOnDomOrgasm
		bEndOnDomOrgasm = false 
		OStim.EndOnDomOrgasm = true 
	endif 
	if bEndOnSubOrgasm
		bEndOnSubOrgasm = false 
		OStim.EndOnSubOrgasm = true 
	endif 
endevent

float[] previousModifiers
Function CalculateStimMultipliers()
	OSexIntegrationMain OStim = OUtils.GetOStim()

	int i = 0
	int max = ActiveSceneActors.Length
	while i < max 
		float arousal = OSLArousedNative.GetArousal(ActiveSceneActors[i])

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
		;console("Modding stim mult for: " + actors[i] + ": " + (modifyBy - previousModifiers[i]))
		previousModifiers[i] = modifyBy

		i += 1
	endwhile
EndFunction

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


; ========== DEBUG RELATED ==================

function Log(string msg) global
    Debug.Trace("---OSLAroused--- [OStimAdapter] " + msg)
endfunction
