ScriptName OSLAroused_AdapterDebug Extends Quest Hidden

OSLAroused_ArousalDisplayWidget Property ArousalDisplay Auto

Function LoadAdapter()
    RegisterForKey(59) ; F1
    RegisterForKey(60) ; F2
    RegisterForKey(61) ; F3
    RegisterForKey(62) ; F4
    RegisterForKey(63) ; F5

    RegisterForKey(34) ; G
    RegisterForKey(35) ; H

    ArousalDisplay.InitializeText()
EndFunction

Event OnKeyDown(int keyCode)
    if(keyCode == 59)       ;F1
        float arousal = SimulateArousalGain()
    elseif(keycode == 60)   ;F2
        SimulateArousalLoss()
    elseif(keycode == 61)   ;F3
        QueryArousal()
    elseif(keycode == 62)   ;F4
    elseif(keycode == 63)   ;F5
    elseif(keycode == 34)   ;G
        TryInitiateSex()
    elseif(keycode == 35)   ;H
        RaiseStim()
    endif
EndEvent

float Function SimulateArousalGain()
    Actor target = Game.GetCurrentCrosshairRef() as Actor
    if(target == none)
        target = Game.GetPlayer()
    endif

    return OSLAroused_ModInterface.ModifyArousal(target, 5.0, "Debug Gain Arousal")
EndFunction

Function SimulateArousalLoss()
    Actor target = Game.GetCurrentCrosshairRef() as Actor
    if(target == none)
        target = Game.GetPlayer()
    endif

    OSLAroused_ModInterface.ModifyArousal(target, -5.0, "Debug Reduce Arousal")
EndFunction

Function QueryArousal()
    Actor target = Game.GetCurrentCrosshairRef() as Actor
    if(target == none)
        target = Game.GetPlayer()
    endif
    OSLAroused_ModInterface.GetArousal(target)
EndFunction

Function OnPlayerArousalUpdated(float arousal)
    ArousalDisplay.SetArousalValue(arousal as int)
endfunction

Function TryInitiateSex()
    actor crosshairTarget = Game.GetCurrentCrosshairRef() as Actor
    if(crosshairTarget)
        OSexIntegrationMain ostim = OUtils.GetOStim()
        ostim.StartScene(crosshairTarget, Game.GetPlayer())
    endif
EndFunction

Function RaiseStim()
    OSexIntegrationMain ostim = OUtils.GetOStim()
    ostim.SetActorExcitement(Game.GetPlayer(), 90)
EndFunction