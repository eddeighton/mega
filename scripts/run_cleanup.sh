#!/bin/bash

#  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
#  Author: Edward Deighton
#  License: Please see license.txt in the project root folder.
#
#  Use and copying of this software and preparation of derivative works
#  based upon this software are permitted. Any copy of this software or
#  of any derivative work must include the above copyright notice, this
#  paragraph and the one after it.  Any distribution of this software or
#  derivative works must comply with all applicable laws.
#
#  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
#  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
#  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
#  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
#  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
#  OF THE POSSIBILITY OF SUCH DAMAGES.


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
cleanup_folder "${BUILD_PATH}/${CFG_TUPLE}/basic/install/bin/log"
cleanup_folder "${WORKSPACE_ROOT_PATH}/log"
cleanup_folder "${WORKSPACE_ROOT_PATH}/src/mega/log"
cleanup_folder "${WORKSPACE_ROOT_PATH}/src/basic/log"

# temporary file folders
cleanup_folder "/tmp/mega_registry"
cleanup_folder "/tmp/megaenv"
cleanup_folder "${BUILD_PATH}/${CFG_TUPLE}/basic/install/tmp"


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
cleanup_files "${BUILD_PATH}/${CFG_TUPLE}/basic"     "*.log"
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
