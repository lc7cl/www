INCDIR := include
SRCDIR := src
OBJDIR := obj
BINDIR := bin
CFLAGS:= -g -I$(INCDIR)

objs:= $(OBJDIR)/uio.o $(OBJDIR)/edns.o $(OBJDIR)/command.o $(OBJDIR)/dnstool.o 
bin := $(BINDIR)/dnstool
output := $(objs) $(bin)
#VPATH := include:src
vpath %.c $(SRCDIR)
vpath %.h $(INCDIR)

default:$(bin)

$(bin):$(objs)
	gcc -o $@ $(objs)

$(objs):$(addprefix $(SRCDIR)/,$(patsubst %.o,%.c,$(notdir $(objs))))
	gcc -c $(CFLAGS) $< -o $@
#uio.o:uio.c uio.h edns.h
#	gcc -c $(CFLAGS) $< 
#edns.o:edns.c uio.h edns.h
#	gcc -c $(CFLAGS) $<
#dnstool.o:dnstool.c dnstool.h uio.h edns.h
#	gcc -c $(CFLAGS) $<
	
clean:
	@rm -rf $(wildcard $(output))
