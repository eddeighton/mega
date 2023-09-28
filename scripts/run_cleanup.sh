#!/bin/bash

source ${WORKSPACE_ROOT_PATH}/src/mega/src/scripts/util_cleanup.sh

# stash folders
cleanup_folder "${BUILD_PATH}/${CFG_TUPLE}/mega/build/stash"
cleanup_folder "${BUILD_PATH}/${CFG_TUPLE}/mega/build/tests/database/advanced/stash"
cleanup_folder "${BUILD_PATH}/${CFG_TUPLE}/mega/build/tests/database/basic/stash"
cleanup_folder "${BUILD_PATH}/${CFG_TUPLE}/mega/install/bin/stash"

# log folders
cleanup_folder "${BUILD_PATH}/${CFG_TUPLE}/mega/install/bin/log"
cleanup_folder "${BUILD_PATH}/${CFG_TUPLE}/basic/install/bin/log"
cleanup_folder "${WORKSPACE_ROOT_PATH}/log"
cleanup_folder "${WORKSPACE_ROOT_PATH}/src/mega/log"
cleanup_folder "${WORKSPACE_ROOT_PATH}/src/basic/log"

# temporary file folders
cleanup_folder "/tmp/mega_registry"
cleanup_folder "/tmp/megaenv"
cleanup_folder "/tmp/invocation_tests"
cleanup_folder "${BUILD_PATH}/${CFG_TUPLE}/basic/install/tmp"
cleanup_folder "${BUILD_PATH}/${CFG_TUPLE}/game/install/tmp"

# log files
cleanup_files "${BUILD_PATH}/${CFG_TUPLE}/mega"         "*.log"
cleanup_files "${BUILD_PATH}/${CFG_TUPLE}/basic"        "*.log"
cleanup_files "${WORKSPACE_ROOT_PATH}/src"              "*.log"

# shared memory files
cleanup_shm "memory_*Leaf*"
cleanup_shm "sem.MEGA_RUNTIME_ADDRESS_SPACE_MUTEX"
