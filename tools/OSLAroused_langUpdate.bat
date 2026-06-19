@echo off
REM Dev helper: regenerate every localized translation file from the English source.
REM Lives in tools/ (outside contrib/Distribution) so it is never shipped to users.
REM Run from anywhere; it operates on the Translations folder relative to this script.
setlocal
set "TRANS=%~dp0..\contrib\Distribution\Assets\interface\Translations"
pushd "%TRANS%" || (echo Could not find Translations folder: "%TRANS%" & exit /b 1)

copy /y OSLAroused_ENGLISH.txt OSLAroused_CHINESE.txt
copy /y OSLAroused_ENGLISH.txt OSLAroused_CZECH.txt
copy /y OSLAroused_ENGLISH.txt OSLAroused_FRENCH.txt
copy /y OSLAroused_ENGLISH.txt OSLAroused_GERMAN.txt
copy /y OSLAroused_ENGLISH.txt OSLAroused_ITALIAN.txt
copy /y OSLAroused_ENGLISH.txt OSLAroused_JAPANESE.txt
copy /y OSLAroused_ENGLISH.txt OSLAroused_POLISH.txt
copy /y OSLAroused_ENGLISH.txt OSLAroused_RUSSIAN.txt
copy /y OSLAroused_ENGLISH.txt OSLAroused_SPANISH.txt
copy /y OSLAroused_ENGLISH.txt OSLAroused_KOREAN.txt
copy /y OSLAroused_ENGLISH.txt OSLAroused_PORTUGUESE.txt

popd
endlocal
