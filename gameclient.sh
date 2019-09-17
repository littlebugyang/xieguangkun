#!/bin/bash
cd "$(dirname "$0")"
chmod +x build/client
nohup ./build/client $1 $2 $3 > nohup.log 2>&1 &
