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

./root --level trace --console warn &

sleep 1

./daemon --level trace --console warn &

sleep 1

./worker --level trace --console warn &

