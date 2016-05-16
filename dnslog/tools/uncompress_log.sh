#!/bin/bash
for file in /data/dns/log/*
do
	fname=`basename ${file}`
    if [ "${fname##*.}" == "gz" ] && [ `lsof ${file} | wc -l` -eq 0 ];then
		gunzip -c ${file} > /data/dns/misc/`basename ${file} ".gz"`
        rm -f ${file}
	fi
done
