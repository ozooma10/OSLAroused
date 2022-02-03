scriptname OSLArousedNative hidden

;Retrieves arousal value for a given Actor
;In SLAroused mode, This is a calculated value based off exposure and time rate
float function GetArousal(Actor actor) global native
;Retrieves arousal for passed in actors. Results in *same* order as passed in array
float[] function GetArousalMultiple(Actor[] actorArray) global native

;Sets the arousal value for a given actor
;In SLAroused Mode this is the Exposure Value (Arousal is calculated)
function SetArousal(Actor actor, float value) global native
Function SetArousalMultiple(Actor[] actorArray, float value) global native

;Modifies the arousal value by the specified amount (Relative to current arousal value)
function ModifyArousal(Actor actor, float value) global native

;Modifies all actors in array by sepcified amount (relative to current arousal)
Function ModifyArousalMultiple(Actor[] actorArray, float value) global native

;Sets the arousal multiplier for a given actor
function SetArousalMultiplier(Actor actor, float value) global native

;Gets the actors current arousal multiplier
float function GetArousalMultiplier(Actor actor) global native

;Gets the "Raw" Exposure value 
;In OAroused mode this is equavalent to GetArousal
float function GetExposure(Actor actor) global native

;Gets the number of days since this actor last orgasmed
float function GetDaysSinceLastOrgasm(Actor actor) global native

function SetTimeRate(Actor actor, float value) global native

;Gets the Actors current time rate
float function GetTimeRate(Actor actor) global native

;Informs dll if player is in a sex scene
function SetPlayerInSexScene(bool value) global native

; ==================== SETTINGS =========================

;Sets weather PlayerNudityCheck should be enabled or disabled in SKSE module
function UpdatePlayerNudityCheck(bool enabled) global native

;Sets Amount to increase arousal per game hour when exposed to nudity
function UpdateHourlyNudityArousalModifier(float arousalMod) global native

;Sets Which arousal mode to use
function UpdateArousalMode(int newArousalMode) global native
 
;Sets Which arousal mode to use
function UpdateDefaultArousalMultiplier(float newMultiplier) global native


; ==================== KEYWORDS =========================

bool function AddKeywordToForm(Form form, Keyword keyword) global native

;====== DEBUG ============

function DumpArousalData() global native
function ClearSecondaryArousalData() global native
function ClearAllArousalData() global native