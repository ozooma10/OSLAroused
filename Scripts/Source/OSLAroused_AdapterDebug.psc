ScriptName OSLAroused_AdapterDebug Extends Quest Hidden

OSLAroused_Main Property Main Auto
Actor Property PlayerRef Auto

Function LoadAdapter()
    RegisterForKey(59) ; F1
    RegisterForKey(60) ; F2
    RegisterForKey(61) ; F3
    RegisterForKey(62) ; F4
    RegisterForKey(63) ; F5
EndFunction

Event OnKeyDown(int keyCode)
    if(keyCode == 59)       ;F1
        SimulateArousalGain()
    elseif(keycode == 60)   ;F2
        SimulateArousalLoss()
    elseif(keycode == 61)   ;F3
        QueryArousal()
    elseif(keycode == 62)   ;F4
    elseif(keycode == 63)   ;F5
    endif
EndEvent

Function SimulateArousalGain()
    Actor target = Game.GetCurrentCrosshairRef() as Actor
    if(target == none)
        target = PlayerRef
    endif

    Main.ModifyArousal(target, 10.0)
EndFunction

Function SimulateArousalLoss()
    Actor target = Game.GetCurrentCrosshairRef() as Actor
    if(target == none)
        target = PlayerRef
    endif

    Main.ModifyArousal(target, -5.0)
EndFunction

Function QueryArousal()
    Actor target = Game.GetCurrentCrosshairRef() as Actor
    if(target == none)
        target = PlayerRef
    endif

    OSLAroused_Main.Log(target.GetDisplayName()+ " Arousal Is: " + Main.GetArousal(target))
EndFunction
