# Name of the project and uproject file - 
# eg should have a PROJECT_NAME.uproject file in the top level
PROJECT_NAME=AdventureTemplate

# Directory inside the container where the project tree is mounted
# and this is where the PROJECT_NAME.uproject file should be
PROJECT_DIR=/project

# Allow for GitHub actions
if [[ -n $GITHUB_WORKSPACE ]]; then
  PROJECT_DIR="${GITHUB_WORKSPACE}"
fi 

# Needed to prevent the dotNet environment crashing due to memory protection issues
export DOTNET_EnableWriteXorExecute=0

# Path to this directory inside the build container - add our config into this container's config
SCRIPT_PATH=$(dirname $0)
CONFIG_FILE="${SCRIPT_PATH}/BuildConfiguration.xml"
cat $CONFIG_FILE > "/home/ue4/.config/Unreal Engine/UnrealBuildTool/BuildConfiguration.xml"

# Path to the build scripts in the containerised linux Unreal Engine install
UE_BINARIES_FILES="/home/ue4/UnrealEngine/Engine/Binaries/Linux"

mkdir -p "{PROJECT_DIR}/Logs"
mkdir -p "{PROJECT_DIR}/Reports"

TIMESTAMP=$(date "+%Y-%m0%d_%H0%M-%S")

/home/ue4/UnrealEngine/Engine/Binaries/Linux/UnrealEditor "${PROJECT_DIR}/${PROJECT_NAME}.uproject" \
   -execcmds="Automation RunTests AdventureGame.Dialog.ConversationDataTest+Private.Tests.BarkTextMultiLineTest;Quit" \
   -stdout -unattended -NOSPLASH -AllowStdOutLogVerbosity -NullRHI \
   -ReportExportPath="${PROJECT_DIR}/Reports" \
   -AbsLog="${PROJECT_DIR}/Logs" \
   -TestExit="Automation Test Queue Empty"
