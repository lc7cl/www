
CC = gcc
CFLAGS = -g -O0 -lpcap -lpthread

OBJS = fifo.o task.o config.o log.o buffer.o dns.o packet.o dnsmirror.o
APP = dnsmirror

.PHONY := clean

default:${OBJS}
	@${CC} ${CFLAGS} -o ${APP} ${OBJS}

%o:%.c
	@${CC} -c %<

clean:
	@rm -f ${OBJS} ${APP}
