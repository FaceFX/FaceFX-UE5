#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

UE_DIR="/Users/Shared/Epic Games/UE_5.8/Engine"

"$UE_DIR/Build/BatchFiles/RunUAT.sh" BuildPlugin \
  -Plugin="$SCRIPT_DIR/FaceFX.uplugin" \
  -Package="$SCRIPT_DIR/Packages/Mac/FaceFX" \
  -Installed \
  -NoCompile \
  -TargetPlatforms=Mac \
  -Architecture_Mac=arm64+x86_64
