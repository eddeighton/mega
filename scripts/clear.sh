#/bin/bash

source ${WORKSPACE_ROOT_PATH}/src/mega/src/scripts/util_cleanup.sh

LOCAL_DIR="/home/${USER}/test_${CFG_TYPE}"

# echo "Clearing log and stash folders from local dir: ${LOCAL_DIR}"

cleanup_folder "${LOCAL_DIR}/log*"
cleanup_folder "${LOCAL_DIR}/events"
cleanup_folder "${LOCAL_DIR}/stash"
cleanup_files  "${LOCAL_DIR}" "symbols.xml"

# echo "Clearing temporary files"

cleanup_folder "/tmp/mega_registry"
cleanup_folder "/tmp/megaenv"
cleanup_folder "${BUILD_PATH}/${CFG_TUPLE}/basic/install/tmp"
cleanup_folder "${BUILD_PATH}/${CFG_TUPLE}/game/install/tmp"

