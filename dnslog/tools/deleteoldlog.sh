#!/bin/bash
find /home/dns/log -mtime +2 -type f -print0|xargs -r0 rm -rf
