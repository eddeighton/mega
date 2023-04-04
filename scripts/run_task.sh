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

PipelineResult=$1
TaskName=$2
SourceName=$3

echo "mega -x --configuration ${BUILD_PATH}/${CFG_TUPLE}/game/build/Game_pipeline.xml --result_in ${PipelineResult} --local --stash_dir ./stash/ --toolchain_xml ${BUILD_PATH}/${CFG_TUPLE}/mega/install/etc/toolchain.xml --task ${TaskName} --source ${SourceName}"

mega -x --configuration ${BUILD_PATH}/${CFG_TUPLE}/game/build/Game_pipeline.xml --result_in ${PipelineResult} --local --stash_dir ./stash/ --toolchain_xml ${BUILD_PATH}/${CFG_TUPLE}/mega/install/etc/toolchain.xml --task ${TaskName} --source ${SourceName}
