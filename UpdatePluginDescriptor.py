# Updates FaceFX.uplugin

import subprocess
import sys

if __name__ == "__main__":
    if sys.version_info.major < 3:
        print("UpdatePluginDescriptor.py requires Python 3")
        exit(1)
        
    if len(sys.argv) < 4:
        print("Usage: python UpdatePluginDescriptor.py <UPLUGIN_FILE> <UE_VERSION> <PLUGIN_VERSION>")
        sys.exit(1)

    uplugin_path = sys.argv[1]
    ue_version = sys.argv[2]
    plugin_version = sys.argv[3]

    with open(uplugin_path, 'r') as uplugin_file:
        uplugin_lines = uplugin_file.readlines()
    for i, line in enumerate(uplugin_lines):
        if 'VersionName' in line:
            uplugin_lines[i] = f'\t"VersionName": "{plugin_version}",\n'
        if 'EngineVersion' in line:
            uplugin_lines[i] = f'\t"EngineVersion": "{ue_version}.0",\n'
    with open(uplugin_path, 'w') as uplugin_file:
        uplugin_file.writelines(uplugin_lines)

    print(f'Updated {uplugin_path}')
