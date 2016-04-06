#!/bin/bash

LOGDIR=/data/l11/dns

if [ `date -d -5min +%H` -ne `date  +%H` ];then
	logrotate -f /home/lichang1/dns/tools/logrotate.conf >/dev/null 2>&1
        FILENAME=/data/l11/dns/`date +%Y%m%d%H`.`hostname`.mirror.log.gz
	for f in `find ${LOGDIR}/ -name "*.gz" -type f ! -path "rsync"`
	do
		fname=`basename $f`
		mv -f $f ${LOGDIR}/rsync/`date +%Y%m%d%H`.`hostname`.${fname%.*}.mirror.log.gz
		
	done
	RSYNC_PASSWORD=12306  rsync -aztr --timeout=3000 ${LOGDIR}/rsync/*.mirror.log.gz root@10.69.32.47::dnsmirror
        
        if [ $? -eq 0  ];then
		rm -f ${LOGDIR}/rsync/*.mirror.log.gz
	fi
fi
