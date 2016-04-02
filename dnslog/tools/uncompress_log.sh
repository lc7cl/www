#!/bin/bash
set -x
for file in /home/dns/log/*
do
	fname=`basename ${file}`
	if [ "${fname##*.}" == "gz" ];then
		zcat /home/dns/log/${fname} > /home/dns/misc/`basename ${file} ".gz"`
	fi
done
