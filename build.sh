#!/bin/bash

set -u

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

SUPPORTED_UE_VERSIONS="5.5,5.6,5.7,5.8"
LATEST_UE_VERSION="5.8"

UE_VERSION="${1:-$LATEST_UE_VERSION}"

IFS=',' read -r -a SUPPORTED_UE_VERSION_ARRAY <<< "$SUPPORTED_UE_VERSIONS"

VERSION_OK=0

for v in "${SUPPORTED_UE_VERSION_ARRAY[@]}"; do
    if [[ "$v" == "$UE_VERSION" ]]; then
        VERSION_OK=1
        break
    fi
done

if [[ "$VERSION_OK" -ne 1 ]]; then
    echo "ERROR: Unsupported Unreal Engine version \"$UE_VERSION\"."
    echo "Supported versions: $SUPPORTED_UE_VERSIONS"
    exit 1
fi

UE_DIR="/Users/Shared/Epic Games/UE_${UE_VERSION}/Engine"
UAT_SH="$UE_DIR/Build/BatchFiles/RunUAT.sh"
PLUGIN_FILE="$SCRIPT_DIR/FaceFX.uplugin"
PACKAGE_DIR="$SCRIPT_DIR/Packages/Mac/FaceFX_UE${UE_VERSION}"

# Detect Runtime version from ./Source/FaceFXLib/
RUNTIME_ROOT="$SCRIPT_DIR/Source/FaceFXLib"
RUNTIME_PREFIX="facefx-runtime-"
RUNTIME_VERSION=""

# Find first directory starting with facefx-runtime-
RUNTIME_DIRNAME=$(find "$RUNTIME_ROOT" -maxdepth 1 -type d -name "${RUNTIME_PREFIX}*" -printf '%f\n' | head -n 1)

if [[ -z "$RUNTIME_DIRNAME" ]]; then
    echo "ERROR: No \"${RUNTIME_PREFIX}*\" directory found in \"$RUNTIME_ROOT\"."
    exit 1
fi

# Strip prefix to get version (e.g. 4.6.0 from facefx-runtime-4.6.0)
RUNTIME_VERSION="${RUNTIME_DIRNAME#${RUNTIME_PREFIX}}"

PACKAGE_DIR="${PACKAGE_DIR}-${RUNTIME_VERSION}"

if [[ ! -f "$PLUGIN_FILE" ]]; then
    echo "ERROR: Plugin file not found:"
    echo "  \"$PLUGIN_FILE\""
    exit 1
fi

if [[ ! -f "$UAT_SH" ]]; then
    echo "ERROR: RunUAT.sh not found:"
    echo "  \"$UAT_SH\""
    exit 1
fi

echo "Building FaceFX for UE $UE_VERSION (SGX|FaceFX Runtime $RUNTIME_VERSION)..."
echo "Package directory: \"$PACKAGE_DIR\""

"$UAT_SH" BuildPlugin \
  -Plugin="$PLUGIN_FILE" \
  -Package="$PACKAGE_DIR" \
  -Installed \
  -NoCompile \
  -TargetPlatforms=Mac \
  -Architecture_Mac=arm64+x86_64

BUILD_EXIT=$?

if [[ "$BUILD_EXIT" -ne 0 ]]; then
    echo "ERROR: BuildPlugin failed with exit code $BUILD_EXIT."
    exit "$BUILD_EXIT"
fi

echo "Build completed successfully."
