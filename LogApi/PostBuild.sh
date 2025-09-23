#!/bin/bash

# HLDS Path
hldsPath="/home/eu/Desktop/27020";

# Check HLDS Path
if [ ! -d "$hldsPath" ]; then
    echo "$hldsPath does not exist, exit."
    exit
fi

# Get current directory name
pluginName="${PWD##*/}"

# Get Ethernet address as IPV4
ipAddress="$(ip -4 -o route show scope link | grep -oP '(?<=src )[\d.]+')"

# Executable HLDS with parameters
hldsRunCommand="./hlds_run -console -debug -game cstrike -port 27020 -pingboost 3 -steam -master -secure -bots -timeout 3 +ip $ipAddress +map de_inferno +maxplayers 32 +sys_ticrate 1000";

# cstrike addons dll path
dllPath="$hldsPath/cstrike/addons/${pluginName,,}/dlls";

# Build file
buildFile=$(realpath "./Release/${pluginName,,}_mm.so")

# Kill HLDS Process
pkill -2 -q -f hlds_linux
pkill -2 -q -f hlds_run

# Sleep
sleep 2

# Make if not exists
mkdir -p "$dllPath"

# Copy file
yes | cp -f "$buildFile" "$dllPath"

# Prevent from HLDS found Steam path in my home directory
export HOME=

# Run HLDS
eval "cd $hldsPath && $hldsRunCommand"
