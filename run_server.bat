@echo off

pushd %CD%
cd /d "server"
start gameserver.bat .\map_r2m1.txt 127.0.0.1 6001 
popd