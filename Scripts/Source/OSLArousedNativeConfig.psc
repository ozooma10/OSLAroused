scriptname OSLArousedNativeConfig hidden

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

;Sets amount of Arousal Baseline to gain when wearing erotic armor (and sends keyword to use)
function SetEroticArmorBaseline(float newVal, Keyword eroticKeyword) global native

;Sets amount of Arousal Baseline to gain for a given device type
function SetDeviceTypesBaseline1(float belt, float collar, float legCuffs, float armCuffs, float bra, float gag, float piercingsNipple, float piercingsVaginal, float blindfold, float harness) global native
function SetDeviceTypesBaseline2(float plugVag, float plugAnal, float corset, float boots, float gloves, float hood, float suit, float heavyBondage, float bondageMittens) global native
function SetDeviceTypeBaseline(int deviceTypeId, float newVal) global native