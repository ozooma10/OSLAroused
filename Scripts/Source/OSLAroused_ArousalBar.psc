Scriptname OSLAroused_ArousalBar Extends SKI_WidgetBase Hidden

float	_width			= 250.0
float	_height			= 25.0
int		_primaryColor	= 4259840
int		_secondaryColor	= 3342336
int		_flashColor		= -1
string 	_fillDirection	= "right"
float	_percent		= 0.0

; PROPERTIES --------------------------------------------------------------------------------------

Bool Property BarVisible = true Auto
Bool Property BarPendingFade = false Auto

float property Width
	{Width of the meter in pixels at a resolution of 1280x720. Default: 292.8}
	float function get()
		return _width
	endFunction

	function set(float a_val)
		_width = a_val
		if (Ready)
			UI.InvokeFloat(HUD_MENU, WidgetRoot + ".setWidth", _width)
		endIf
	endFunction
endProperty

float property Height
	{Height of the meter in pixels at a resolution of 1280x720. Default: 25.2}
	float function get()
		return _height
	endFunction

	function set(float a_val)
		_height = a_val
		if (Ready)
			UI.InvokeFloat(HUD_MENU, WidgetRoot + ".setHeight", _height)
		endIf
	endFunction
endProperty

int property PrimaryColor
	{Primary color of the meter gradient RRGGBB [0x000000, 0xFFFFFF]. Default: 0xFF0000. Convert to decimal when editing this in the CK}
	int function get()
		return _primaryColor
	endFunction

	function set(int a_val)
		_primaryColor = a_val
		if (Ready)
			UI.InvokeInt(HUD_MENU, WidgetRoot + ".setColor", _primaryColor)
		endIf
	endFunction
endProperty

int property SecondaryColor
	{Secondary color of the meter gradient, -1 = automatic. RRGGBB [0x000000, 0xFFFFFF]. Default: -1. Convert to decimal when editing this in the CK}
	int function get()
		return _secondaryColor
	endFunction

	function set(int a_val)
		SetColors(_primaryColor, a_val, _flashColor)
	endFunction
endProperty

int property FlashColor
	{Color of the meter warning flash, -1 = automatic. RRGGBB [0x000000, 0xFFFFFF]. Default: -1. Convert to decimal when editing this in the CK}
	int function get()
		return _flashColor
	endFunction

	function set(int a_val)
		_flashColor = a_val
		if (Ready)
			UI.InvokeInt(HUD_MENU, WidgetRoot + ".setFlashColor", _flashColor)
		endIf
	endFunction
endProperty

string property FillDirection
	{The position at which the meter fills from, ["left", "center", "right"] . Default: center}
	string function get()
		return _fillDirection
	endFunction

	function set(string a_val)
		_fillDirection = a_val
		if (Ready)
			UI.InvokeString(HUD_MENU, WidgetRoot + ".setFillDirection", _fillDirection)
		endIf
	endFunction
endProperty

float property Percent
	{Percent of the meter [0.0, 1.0]. Default: 0.0}
	float function get()
		return _percent
	endFunction

	function set(float a_val)
		_percent = a_val
		if (Ready)
			UI.InvokeFloat(HUD_MENU, WidgetRoot + ".setPercent", _percent)
		endIf
	endFunction
endProperty

; EVENTS ------------------------------------------------------------------------------------------

; @override SKI_WidgetBase
event OnWidgetReset()

	parent.OnWidgetReset()

	; Init numbers
	float[] numberArgs = new float[6]
	numberArgs[0] = _width
	numberArgs[1] = _height
	numberArgs[2] = _primaryColor as float
	numberArgs[3] = _secondaryColor as float
	numberArgs[4] = _flashColor as float
	numberArgs[5] = _percent
	UI.InvokeFloatA(HUD_MENU, WidgetRoot + ".initNumbers", numberArgs)

	; Init strings
	string[] stringArgs = new string[1] ;This is an array because I want to add more to it later...
	stringArgs[0] = _fillDirection
	UI.InvokeStringA(HUD_MENU, WidgetRoot + ".initStrings", stringArgs)

	; Init commit
	UI.Invoke(HUD_MENU, WidgetRoot + ".initCommit")
endEvent

; FUNCTIONS ---------------------------------------------------------------------------------------

; @overrides SKI_WidgetBase
string function GetWidgetSource()
	return "skyui/meter.swf"
endFunction

; @overrides SKI_WidgetBase
string function GetWidgetType()
 return "ArousalBarWidget"
endFunction

function SetPercent(float a_percent, bool a_force = false)
	{Sets the meter percent, a_force sets the meter percent without animation}
	_percent = a_percent
	if (Ready)
		float[] args = new float[2]
		args[0] = a_percent
		args[1] = a_force as float
		UI.InvokeFloatA(HUD_MENU, WidgetRoot + ".setPercent", args)
	endIf
endFunction

function ForcePercent(float a_percent)
	{Convenience function for SetPercent(a_percent, true)}
	SetPercent(a_percent, true)
endFunction


function StartFlash(bool a_force = false)
	{Starts meter flashing. a_force starts the meter flashing if it's already animating}
	if (Ready)
		UI.InvokeBool(HUD_MENU, WidgetRoot + ".startFlash", a_force)
	endIf
endFunction


function ForceFlash()
	{Convenience function for StartFlash(true)}
	StartFlash(true)
endFunction

function SetColors(int a_primaryColor, int a_secondaryColor = -1, int a_flashColor = -1)
	{Sets the meter percent, a_force sets the meter percent without animation}
	_primaryColor = a_primaryColor;
	_secondaryColor = a_secondaryColor;
	_flashColor = a_flashColor;

	if (Ready)
		int[] args = new int[3]
		args[0] = a_primaryColor
		args[1] = a_secondaryColor
		args[2] = a_flashColor
		UI.InvokeIntA(HUD_MENU, WidgetRoot + ".setColors", args)
	endIf
endFunction

function TransitionColors(int a_primaryColor, int a_secondaryColor = -1, int a_flashColor = -1, int a_duration = 1000)
	{Sets the meter percent, a_force sets the meter percent without animation}
	_primaryColor = a_primaryColor;
	_secondaryColor = a_secondaryColor;
	_flashColor = a_flashColor;

	if (Ready)
		int[] args = new int[4]
		args[0] = a_primaryColor
		args[1] = a_secondaryColor
		args[2] = a_flashColor
		args[3] = a_duration
		UI.InvokeIntA(HUD_MENU, WidgetRoot + ".transitionColors", args)
	endIf
endFunction

Function InitializeBar(float initialPercent)
	HAnchor = "left"
	VAnchor = "bottom"

	X = 980.0
	Y = 160.0

	Alpha = 0.0
	SetPercent(initialPercent)
	FillDirection = "Left"

	SetColors(0xdc143c, 0xF6C4CE)

	SetBarVisible(False)
EndFunction

function DisplayBarWithAutohide(float hideDelay)
	RegisterForSingleUpdate(hideDelay)
	BarPendingFade = true
	SetBarVisible(true)
endfunction

function SetBarVisible(bool newVisibility)
    if(newVisibility)
        FadeTo(100.0, 1.0)
        BarVisible = true
    else
        FadeTo(0.0, 1.0)
        BarVisible = false
		BarPendingFade = false
    endif
Endfunction

event OnUpdate()
	if(BarPendingFade && BarVisible)
		SetBarVisible(false)
	endif
endevent