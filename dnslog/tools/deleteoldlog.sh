#!/bin/bash
find /data/dns/log -mtime +1 -type f -print0|xargs -r0 rm -rf
