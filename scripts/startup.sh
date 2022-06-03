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

./root --level debug --console error &

sleep 1

./daemon --level debug --console error &

sleep 1

./worker --level debug --console error &

