CC = g++ 
CFLAGS = -g -O0
RM = rm 

OBJS = logdir.o test.o
BIN = test

.PHONY = clean

default:${OBJS}
	@${CC} ${CFLAGS} -o ${BIN} $^

%o:%c
	@${CC} -c ${CFLAGS} -o $@ $<

clean:
	@${RM} -rf ${OBJS} ${BIN}
