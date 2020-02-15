#!/bin/sh

PROCESS_ID=`ps -ef | grep "ExplorerTest-1.0-SNAPSHOT"  | grep -v "grep" | awk '{print $2}'`

echo "Stop ExplorerTest-1.0-SNAPSHOT on pid " $PROCESS_ID " at " `date`
kill -9 $PROCESS_ID

