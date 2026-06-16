scriptname OSLArousedNativeConfig hidden

;Sets the minimum value libido can fall to
function SetMinLibidoValue(bool bPlayerVal, float newVal) global native

;Sets percentage change of arousal over time
function SetArousalChangeRate(float newVal) global native

;Sets percentage change of libido over time
function SetLibidoChangeRate(float newVal) global native

;Sets Amount of Arousal Baseline to gain when partcipating in scene
function SetSceneParticipantBaseline(float newVal) global native

;Sets Amount of Arousal Baseline to gain when Spectating a scene
function SetSceneViewingBaseline(float newVal) global native

;Sets If scene victims should gain arousal from scene activity
function SetSceneVictimGainsArousal(bool newVal) global native

;Sets Amount of Arousal Baseline to gain when nud
function SetBeingNudeBaseline(float newVal) global native

;Sets Amount of Arousal Baseline to gain when near nude actor
function SetViewingNudeBaseline(float newVal) global native

;Sets amount of Arousal Baseline to gain for a registered erotic armor keyword
function SetEroticArmorBaseline(float newVal, Keyword eroticKeyword) global native
float function GetEroticArmorBaseline(Keyword eroticKeyword) global native

;Sets amount of Arousal Baseline to gain for a given device type
function SetDeviceTypesBaseline1(float belt, float collar, float legCuffs, float armCuffs, float bra, float gag, float piercingsNipple, float piercingsVaginal, float blindfold, float harness) global native
function SetDeviceTypesBaseline2(float plugVag, float plugAnal, float corset, float boots, float gloves, float hood, float suit, float heavyBondage, float bondageMittens) global native
function SetDeviceTypeBaseline(int deviceTypeId, float newVal) global native

bool function IsInOSLMode() global native
function SetInOSLMode(bool newVal) global native

;SLA Mode Settings
function SetSLATimeRateHalfLife(float newVal) global native
function SetSLADefaultExposureRate(float newVal) global native

float function GetUpdateIntervalRealTimeSeconds() global native

;Sleep effect: arousal the player gains after a full (non-interrupted) sleep. 0 disables.
function SetSleepArousalGain(float newVal) global native
float function GetSleepArousalGain() global native

;OSL mode: direct, lasting arousal an observer gains per interval while seeing nudity. 0 disables.
function SetSpectatorArousalGain(float newVal) global native
float function GetSpectatorArousalGain() global native

;A.N.D. Integration Settings
function SetUseANDIntegration(bool enabled) global native
bool function GetUseANDIntegration() global native
bool function IsANDIntegrationEnabled() global native

;A.N.D. Faction Baseline Settings
;Faction indices: 0=Nude, 1=Topless, 2=Bottomless, 3=ShowingChest, 4=ShowingAss, 5=ShowingGenitals, 6=ShowingBra, 7=ShowingUnderwear
function SetANDFactionBaseline(int factionIndex, float value) global native
float function GetANDFactionBaseline(int factionIndex) global native

; Utilities
String function RoundFloat(float value, int decimals) global native
