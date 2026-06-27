@ECHO OFF

SETLOCAL

SET "SUPPORTED_UE_VERSIONS=5.5,5.6,5.7,5.8"

SET "LATEST_UE_VERSION=5.8"

SET "UE_VERSION=%~1"

IF "%UE_VERSION%"=="" SET "UE_VERSION=%LATEST_UE_VERSION%"

SET "VERSION_OK="

FOR %%V IN (%SUPPORTED_UE_VERSIONS%) DO (
    IF /I "%%~V"=="%UE_VERSION%" SET "VERSION_OK=1"
)

IF NOT DEFINED VERSION_OK (
    ECHO ERROR: Unsupported Unreal Engine version "%UE_VERSION%".
    ECHO Supported versions: %SUPPORTED_UE_VERSIONS%
    EXIT /B 1
)

SET "UE_DIR=C:\Program Files\Epic Games\UE_%UE_VERSION%\Engine\"
SET "UAT_BAT=%UE_DIR%Build\BatchFiles\RunUAT.bat"
SET "PLUGIN_FILE=%~dp0FaceFX.uplugin"
SET "PACKAGE_DIR=%~dp0Packages\Windows\FaceFX"

IF NOT EXIST "%PLUGIN_FILE%" (
    ECHO ERROR: Plugin file not found:
    ECHO   "%PLUGIN_FILE%"
    EXIT /B 1
)

IF NOT EXIST "%UAT_BAT%" (
    ECHO ERROR: RunUAT.bat not found:
    ECHO   "%UAT_BAT%"
    EXIT /B 1
)

ECHO Building FaceFX for UE %UE_VERSION%...

CALL "%UAT_BAT%" BuildPlugin -Plugin="%PLUGIN_FILE%" -Package="%PACKAGE_DIR%" -Installed -NoCompile -TargetPlatforms=Win64

SET "BUILD_EXIT=%ERRORLEVEL%"

IF NOT "%BUILD_EXIT%"=="0" (
    ECHO ERROR: BuildPlugin failed with exit code %BUILD_EXIT%.
    EXIT /B %BUILD_EXIT%
)

ECHO Build completed successfully.

ENDLOCAL
