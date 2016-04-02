
OBJS = refcount.o acl.o radix.o region-allocator.o worker.o
LIBSO = dnsacl.so

default: $(OBJS)
	g++ -shared -fPIC -o $(LIBSO) $(OBJS)

clean:
	@rm -f $(OBJS) $(LIBSO)

.PHONY: clean

%.o: %.c
	g++ -fpic -c $<
