#!/bin/bash
set -x
for file in /data/dns/log/*
do
	fname=`basename ${file}`
	if [ "${fname##*.}" == "gz" ];then
		zcat ${file} > /home/dns/misc/`basename ${file} ".gz"`
		rm -f ${file}
	fi
done
