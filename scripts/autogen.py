# Projekt RETI python autogen script
# This script will invoke other scripts

import subprocess
import sys
import os

if __name__ == "__main__":
    # Validate arguments
    if len(sys.argv) > 1:
        # Extract action and args
        action = sys.argv[1]
        args = sys.argv[2::]

        # Invoke script
        actionScriptPath = f"./scripts/actions/{action}.py"
        if os.path.exists(actionScriptPath):
            subprocess.run(['py', f'{actionScriptPath}', *args])
        else:
            print(f"Action with the name '{actionScriptPath}' not found!")
    else:
        print(f"Usage: autogen <action> [args...]")

