scriptname OSLAroused_Debug Hidden

function ShowDebugStatusMenu(Actor akRef) global
    uilistmenu ListMenu = uiextensions.GetMenu("UIListMenu") as uilistmenu 
    ListMenu.AddEntryItem("==== " + akRef.GetDisplayName() + " =====")
    ListMenu.AddEntryItem("Arousal: " + OSLArousedNative.GetArousal(akRef))
    ListMenu.AddEntryItem("Baseline: " + OSLArousedNative.GetArousalBaseline(akRef))
    ListMenu.AddEntryItem("Libido: " + OSLArousedNative.GetLibido(akRef))
    ListMenu.OpenMenu()
endfunction