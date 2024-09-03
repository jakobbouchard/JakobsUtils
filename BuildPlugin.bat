@echo off
set "UNREAL_VERSION=5.4"
set /p "UNREAL_VERSION=Enter a version number or press ENTER for default [%UNREAL_VERSION%]: "

set "PLUGIN_LOCATION=%~dp0JakobsUtils.uplugin"
set "OUTPUT_LOCATION=%~dp0Build"
set "PLATFORMS=Win64"

"C:\Program Files\Epic Games\UE_%UNREAL_VERSION%\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin ^
-plugin=%PLUGIN_LOCATION% -package=%OUTPUT_LOCATION%
