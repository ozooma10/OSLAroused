scriptname OSLArousedNative hidden

;Sets weather PlayerNudityCheck should be enabled or disabled in SKSE module
function UpdatePlayerNudityCheck(bool enabled) global native

;Retrieves arousal value for a given Actor
float function GetArousal(Actor actor) global native
;Retrieves arousal for passed in actors. Results in *same* order as passed in array
float[] function GetArousalMultiple(Actor[] actorArray) global native

;Sets the arousal value for a given actor
function SetArousal(Actor actor, float value) global native
Function SetArousalMultiple(Actor[] actorArray, float value) global native

;Modifies the arousal value by the specified amount (Relative to current arousal value)
function ModifyArousal(Actor actor, float value) global native

;Modifies all actors in array by sepcified amount (relative to current arousal)
Function ModifyArousalMultiple(Actor[] actorArray, float value) global native

;Sets the arousal multiplier for a given actor
function SetArousalMultiplier(Actor actor, float value) global native