
CFLAGS:= -g
objs:=uio.o edns.o dnstool.o
output := $(objs) dnstool

default:dnstool

dnstool:$(objs)
	gcc -o $@ $(objs)

uio.o:uio.c uio.h edns.h
	gcc -c $(CFLAGS) $^
edns.o:edns.c uio.h edns.h
	gcc -c $(CFLAGS) $^
dnstool.o:dnstool.c dnstool.h uio.h edns.h
	gcc -c $(CFLAGS) $^
	
clean:
	@rm -rf $(wildcard $(output))
