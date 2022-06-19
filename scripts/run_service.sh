#!/bin/bash

if [ x"${BUILD_PATH}" == "x" ]; then
    echo "BUILD_PATH is not configured"
    exit 1
fi

if [ x"${CFG_TUPLE}" == "x" ]; then
    echo "CFG_TUPLE is not configured"
    exit 1
fi

rm -rf ./log

./root --level info --console error &

sleep 1

./daemon --level info --console error &

sleep 1

./executor --level info --console error &

tail -f ./log/root* | egrep -h "SUCCESS|CACHE|FAIL|error" --color=none

