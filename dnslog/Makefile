CC = g++ 
CFLAGS = -g -O0 
LDFLAGS = -lboost_filesystem -lboost_system -lboost_thread
RM = rm 

OBJS = logdir.o logwatcher.o logstream.o logdb.o logtask.o main.o
BIN = test

.PHONY = clean

default:${OBJS}
	@${CC} ${CFLAGS} ${LDFLAGS} -o ${BIN} $^

%o:%cc
	@${CC} -c ${CFLAGS} -o $@ $<

clean:
	@${RM} -rf ${OBJS} ${BIN}
