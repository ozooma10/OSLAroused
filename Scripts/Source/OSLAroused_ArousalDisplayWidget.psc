Scriptname OSLAroused_ArousalDisplayWidget Extends SKI_WidgetBase Hidden

bool VisibleVal = false
int Value = 0

bool Property Visible
	bool function get()
		return VisibleVal
	endFunction

	function set(bool val)
		VisibleVal = val
		if (Ready)
			UI.InvokeBool(HUD_MENU, WidgetRoot + ".setVisible", VisibleVal) 
		endIf
	endFunction
endProperty

int property ArousalValue
	int function get()
		return Value
	endFunction

	function set(int val)
		Value = val
		if (Ready)
			UI.InvokeInt(HUD_MENU, WidgetRoot + ".setCount", Value) 
		endIf
	endFunction
endProperty

event OnWidgetReset()
	parent.OnWidgetReset()
	
	UI.InvokeBool(HUD_MENU, WidgetRoot + ".setVisible", VisibleVal)
	UI.InvokeInt(HUD_MENU, WidgetRoot + ".setCount", Value)
endEvent

string function GetWidgetSource()
	return "skyui/arrowcount.swf"
endFunction

string function GetWidgetType()
	return "SKI_ArrowCountWidget"
endFunction

Function SetArousalValue(int newVal)
	ArousalValue = newVal
endfunction

Function InitializeText()
	HAnchor = "left"
	VAnchor = "bottom"

	X = 980.0
	Y = 100.0

	Alpha = 100
	Visible = true
	ArousalValue = 0
EndFunction