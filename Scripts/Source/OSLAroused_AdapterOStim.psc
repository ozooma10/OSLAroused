ScriptName OSLAroused_AdapterOStim Extends Quest Hidden

OSLAroused_Main Property Main Auto
Actor Property PlayerRef Auto

bool Property RequireLowArousalToEndScene Auto

OSexIntegrationMain OStim
ODatabaseScript ODatabase

actor[] ActiveSceneActors

function UpdateAdapter()
    if(OStim && OStim.AnimationRunning())
		if(ODatabase.IsSexAnimation(OStim.GetCurrentAnimationOID()))
			Main.ModifyArousalMultiple(ActiveSceneActors, 1.5 * OStim.SexExcitementMult)

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

			Main.ModifyArousalMultiple(PapyrusUtil.RemoveActor(nearby, none), 5.0 * OStim.SexExcitementMult)
		endif
        RegisterForSingleUpdate(15.0)
	endif
endfunction

Event OnUpdate()
    UpdateAdapter()
endevent

Event OStimOrgasm(String EventName, String Args, Float Nothing, Form Sender)
	actor orgasmer = OStim.GetMostRecentOrgasmedActor()

	float reduceBy = (OStim.GetTimeSinceStart() / 120) * OStim.SexExcitementMult
    reduceBy = papyrusutil.ClampFloat(reduceBy, 0.75, 1.5)
    reduceBy = reduceBy * 55.0
    reduceBy = reduceBy + PO3_SKSEFunctions.GenerateRandomFloat(-5.0, 5.0)
    reduceBy = -reduceBy 

	Main.ModifyArousal(orgasmer, reduceBy)

	CalculateStimMultipliers()

	if orgasmer == playerref
		if Main.GetArousal(playerref) < 15
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
	ActiveSceneActors = OStim.GetActors()

	previousModifiers = PapyrusUtil.FloatArray(3)
	CalculateStimMultipliers()

	Main.ModifyArousalMultiple(ActiveSceneActors, 5.0 * OStim.SexExcitementMult)

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
	; increase arousal for actors that did not orgasm
	int i = 0 
	int max = ActiveSceneActors.Length
	while i < max 
		if OStim.GetTimesOrgasm(ActiveSceneActors[i]) < 1
			main.ModifyArousal(ActiveSceneActors[i], 20.0)
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

	int i = 0
	int max = ActiveSceneActors.Length
	while i < max 
		float arousal = main.GetArousal(ActiveSceneActors[i])

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
    if (Game.GetModByName("Ostim.esp"))
        OStim = OUtils.GetOStim()
    endif

    ;If we didnt find OStim, Looks like something went wrong
    if(OStim == none)
        return false
    endif

	odatabase = OStim.GetODatabase()
	if OStim.GetAPIVersion() < 23
		debug.MessageBox("Your OStim version is out of date. OAroused requires a newer version.")
        OStim = none
		return false
	endif

	RegisterForModEvent("ostim_orgasm", "OStimOrgasm")
	RegisterForModEvent("ostim_start", "OStimStart")
	RegisterForModEvent("ostim_end", "OStimEnd")
    return true
endfunction

function Log(string msg) global
    Debug.Trace("---OSLAroused--- " + msg)
endfunction