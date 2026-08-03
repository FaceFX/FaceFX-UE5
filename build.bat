@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION

FOR /F %%I in ('git status --porcelain') DO (
  ECHO ERROR: Repo is dirty before build
  git status --short
  EXIT /B 1
)

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
SET "PACKAGE_DIR=%~dp0Packages\Windows\FaceFX_UE%UE_VERSION%"

REM Detect Runtime version from ./Source/FaceFXLib/

SET "RUNTIME_ROOT=%~dp0Source\FaceFXLib"
SET "RUNTIME_PREFIX=facefx-runtime-"
SET "RUNTIME_VERSION="

REM Find first directory starting with facefx-runtime-

FOR /F "delims=" %%D IN ('DIR /B /AD "%RUNTIME_ROOT%\%RUNTIME_PREFIX%*"') DO (
    SET "RUNTIME_DIRNAME=%%D"
    GOTO :GotRuntimeDir
)

ECHO ERROR: No "%RUNTIME_PREFIX%*" directory found in "%RUNTIME_ROOT%".
EXIT /B 1

:GotRuntimeDir
REM Strip prefix to get version (e.g. 4.6.0 from facefx-runtime-4.6.0)
SET "RUNTIME_VERSION=!RUNTIME_DIRNAME:%RUNTIME_PREFIX%=!"

SET "PACKAGE_DIR=%PACKAGE_DIR%-!RUNTIME_VERSION!"

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

ECHO Preparing to build...

python.exe UpdatePluginDescriptor.py "%PLUGIN_FILE%" "%UE_VERSION%" "!RUNTIME_VERSION!"

SET "PY_EXIT=%ERRORLEVEL%"

IF NOT "%PY_EXIT%"=="0" (
    git restore -- "%PLUGIN_FILE%"
    ECHO ERROR: UpdatePluginDescriptor.py failed with exit code %PY_EXIT%.
    EXIT /B %PY_EXIT%
)

ECHO Building for UE %UE_VERSION% (SGX^|FaceFX Runtime !RUNTIME_VERSION!)...
ECHO Package directory: "%PACKAGE_DIR%"

CALL "%UAT_BAT%" BuildPlugin -Plugin="%PLUGIN_FILE%" -Package="%PACKAGE_DIR%" -Installed -NoCompile -TargetPlatforms=Win64

SET "BUILD_EXIT=%ERRORLEVEL%"

git restore -- "%PLUGIN_FILE%"

IF NOT "%BUILD_EXIT%"=="0" (
    ECHO ERROR: BuildPlugin failed with exit code %BUILD_EXIT%.
    EXIT /B %BUILD_EXIT%
)

ECHO Build completed successfully.

ENDLOCAL
EXIT /B 0
