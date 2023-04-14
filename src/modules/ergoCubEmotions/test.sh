#!/bin/bash

list="angry happy neutral shy wink"

while true; do
	for a in $list; do
		echo "setEmotion $a"
		echo "setEmotion $a"  | yarp rpc /ergoCubEmotions/rpc
		sleep 1
	done
done