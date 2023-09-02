#!/bin/bash

source ${WORKSPACE_ROOT_PATH}/src/mega/src/scripts/util_cleanup.sh

# mega build folders
cleanup_folder "${BUILD_PATH}/${CFG_TUPLE}/game/*"
cleanup_folder "${BUILD_PATH}/${CFG_TUPLE}/basic/*"
