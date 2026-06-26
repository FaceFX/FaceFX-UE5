@ECHO OFF

SET UE_DIR=C:\Program Files\Epic Games\UE_5.8\Engine\

CALL "%UE_DIR%Build\BatchFiles\RunUAT.bat" BuildPlugin -Plugin="%~dp0FaceFX.uplugin" -Package="%~dp0Packages\Windows\FaceFX" -Installed -NoCompile -TargetPlatforms=Win64
