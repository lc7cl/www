INCDIR := include
SRCDIR := src
OBJDIR := obj
BINDIR := bin
CFLAGS:= -g -I$(INCDIR)

objs:= $(OBJDIR)/uio.o $(OBJDIR)/edns.o $(OBJDIR)/command.o $(OBJDIR)/dnstool.o 
objs1:=uio.o edns.o command.o dnstool.o
bin := $(BINDIR)/dnstool
output := $(objs) $(bin) 
#VPATH := include:src
vpath %.c $(SRCDIR)
vpath %.h $(INCDIR)
vpath %.o $(OBJDIR)

default:$(bin)

$(bin):$(objs)
	gcc -o $@ $(objs)

$(OBJDIR)/%.o:%.c
	gcc -c $(CFLAGS) $< -o $@
	
clean:
	@rm -rf $(wildcard $(output))
