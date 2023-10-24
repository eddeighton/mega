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

LOCAL_DIR="/home/${USER}/test_${CFG_TYPE}"

cp ${WORKSPACE_ROOT_PATH}/src/mega/src/src/reports/templates/* ${BUILD_PATH}/${CFG_TUPLE}/mega/install/templates/

