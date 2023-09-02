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

    ## allow wildcard
    folder_path_arg=$1

    folder_path_list=`echo ${folder_path_arg}`
    for folder_path in ${folder_path_list}
    do
        if [[ -d "${folder_path}" ]]
        then
            echo "Removing folder: ${folder_path}"
            rm -rf ${folder_path}
        fi
    done
}
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