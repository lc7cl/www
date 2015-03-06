
BINDIR:=bin
OBJDIR:=obj
SRCDIR:=src
INCDIR:=include

BIN:=$(BINDIR)/log
OBJS:=$(OBJDIR)/log.o
output:=$(BIN) $(OBJS)

CFLAGS:=-g -I$(INCDIR)

vpath %.c $(SRCDIR)
vpath %.h $(INCDIR)


default:prepare $(BIN)

prepare:
	@$(foreach n,$(DIR),$(shell test -d $(n)|| mkdir $(n)))

clean:
	@rm -f $(wildcard $(output))

$(BIN):$(OBJS)
	gcc -o $@ $(OBJS)

$(OBJDIR)/%.o:%.c
	gcc -c $< $(CFLAGS) -o $@




