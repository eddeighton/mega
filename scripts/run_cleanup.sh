#!/bin/bash

if [ x"${BUILD_PATH}" == "x" ]; then
    echo "BUILD_PATH is not configured"
    exit 1
fi

if [ x"${CFG_TUPLE}" == "x" ]; then
    echo "CFG_TUPLE is not configured"
    exit 1
fi

if [ x"${WORKSPACE_ROOT_PATH}" == "x" ]; then
    echo "WORKSPACE_ROOT_PATH is not configured"
    exit 1
fi

function cleanup_folder {

    folder_path=$1

    

    if [[ -d "${folder_path}" ]]
    then
        echo "Removing folder: ${folder_path}"
        rm -rf ${folder_path}
    fi
}

# stash folders
cleanup_folder "${BUILD_PATH}/${CFG_TUPLE}/mega/build/stash"
cleanup_folder "${BUILD_PATH}/${CFG_TUPLE}/mega/build/tests/database/advanced/stash"
cleanup_folder "${BUILD_PATH}/${CFG_TUPLE}/mega/build/tests/database/basic/stash"
cleanup_folder "${BUILD_PATH}/${CFG_TUPLE}/mega/install/bin/stash"

# log folders
cleanup_folder "${BUILD_PATH}/${CFG_TUPLE}/mega/install/bin/log"
cleanup_folder "${BUILD_PATH}/${CFG_TUPLE}/megatest/install/bin/log"
cleanup_folder "${WORKSPACE_ROOT_PATH}/log"
cleanup_folder "${WORKSPACE_ROOT_PATH}/src/mega/log"
cleanup_folder "${WORKSPACE_ROOT_PATH}/src/megatest/log"

# temporary file folders
cleanup_folder "/tmp/mega_registry"
cleanup_folder "/tmp/megaenv"
cleanup_folder "${BUILD_PATH}/${CFG_TUPLE}/megatest/install/tmp"


function cleanup_files {

    folder_path=$1
    file_filter=$2

    pushd ${folder_path} > /dev/null
    for file in `find . -iname "${file_filter}"`
    do
        if [[ -f "${file}" ]]
        then
            echo "Removing file: ${folder_path}/${file}"
            rm ${file}
        fi
    done
    popd > /dev/null

}

cleanup_files "${BUILD_PATH}/${CFG_TUPLE}/mega"         "*.log"
cleanup_files "${BUILD_PATH}/${CFG_TUPLE}/megatest"     "*.log"
cleanup_files "${WORKSPACE_ROOT_PATH}/src"              "*.log"

function cleanup_shm {

    file_filter=$1

    pushd /dev/shm > /dev/null
    for file in `find . -iname "${file_filter}"`
    do
        if [[ -f "${file}" ]]
        then
            echo "Removing file: /dev/shm/${file}"
            rm ${file}
        fi
    done
    popd > /dev/null

}

cleanup_shm "memory_*Leaf*"
cleanup_shm "sem.MEGA_RUNTIME_ADDRESS_SPACE_MUTEX"
