#!/bin/bash

CURDIR=$(pwd)

PART1='
ifeq ($(RTE_SDK),)
	$(error "Must define RTE_SDK")
endif

ifeq ($(RTE_TARGET),)
	$(error "Must define RTE_SDK")
endif

include $(RTE_SDK)/mk/rte.vars.mk

CFLAGS+= -g
'

PART2=
PART3=

PART4='include $(RTE_SDK)/mk/rte.extapp.mk'

function genmk {
    if [ ! -d $1 ];then
        echo "not directory $1"
        return
    fi

    cd $1

    echo "$PART1" > Makefile
    PART2="APP=$1
    "
    echo "$PART2" >> Makefile

    for i in $1/*.c;do
        PART3="${PART3}SRC-y+=$(basename $i)
        "
    done

    echo "$PART3" >> Makefile
    echo "$PART4" >> Makefile
    cd $CURDIR
}

recurse()
{
    for i in $1/*
    do
        if [ -d $i ]
        then
            genmk $(basename $i)
        fi
    done
}

recurse $CURDIR
