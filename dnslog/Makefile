CC = g++ 
CFLAGS = -g -O0 
LDFLAGS = -L/usr/local/lib -L/usr/lib -ldl -lpthread -ljansson -lboost_filesystem  -lboost_system -lboost_thread
RM = rm 

OBJS = logdns.o logdir.o logstream.o logdb.o logtask.o main.o
BIN = test

.PHONY = clean

default:${OBJS}
	@${CC} ${CFLAGS} ${LDFLAGS} -o ${BIN} $^

%o:%cc
	@${CC} -c ${CFLAGS} -o $@ $^

clean:
	@${RM} -rf ${OBJS} ${BIN}
