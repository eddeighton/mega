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

TaskName=$1

echo "mega -x --configuration ${BUILD_PATH}/${CFG_TUPLE}/basic/build/Basic_pipeline.xml --result_in basic_pipeline_result.xml --force_no_stash --local --stash_dir ./stash/ --toolchain_xml ${BUILD_PATH}/${CFG_TUPLE}/mega/install/etc/toolchain.xml --task ${TaskName}"

mega -x --configuration ${BUILD_PATH}/${CFG_TUPLE}/basic/build/Basic_pipeline.xml --result_in basic_pipeline_result.xml --force_no_stash --local --stash_dir ./stash/ --toolchain_xml ${BUILD_PATH}/${CFG_TUPLE}/mega/install/etc/toolchain.xml --task ${TaskName}
