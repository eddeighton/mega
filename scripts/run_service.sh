#!/bin/bash

if [ x"${BUILD_PATH}" == "x" ]; then
    echo "BUILD_PATH is not configured"
    exit 1
fi

if [ x"${CFG_TUPLE}" == "x" ]; then
    echo "CFG_TUPLE is not configured"
    exit 1
fi

${BUILD_PATH}/${CFG_TUPLE}/mega/install/bin/root --level info --console error &

sleep 1

${BUILD_PATH}/${CFG_TUPLE}/mega/install/bin/daemon --level info --console error &

sleep 1

${BUILD_PATH}/${CFG_TUPLE}/mega/install/bin/executor --level info --console error &

tail -f ./log/root* | egrep -h "SUCCESS|CACHE|FAIL|warn|error" --color=none
#tail -f ./log/root* | egrep -h "SUCCESS|CACHE|FAIL|MSG|warn|error" --color=none

#tail -f ./log/*
