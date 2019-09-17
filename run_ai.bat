@echo off

pushd %CD%
cd /d "ai"
start gameclient.bat 1112 127.0.0.1 6001 
popd