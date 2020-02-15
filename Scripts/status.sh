#!/bin/sh

PROCESS_ID=`ps -ef | grep "ExplorerTest-1.0-SNAPSHOT"  | grep -v "grep" | awk '{gsub(/^[ \t\r\n]+|[ \t\r\n]+$/, "", $2); printf $2}'`

if [ ! -z $PROCESS_ID ]
then
	echo "ExplorerTest is running with PID" $PROCESS_ID
	return 1
else
	echo "ExplorerTest is NOT running!"
	return 0
fi

