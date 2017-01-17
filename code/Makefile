CFLAGS	= -lm -g
RM	= rm -f

default:
	$(CC) $(CFLAGS) -o the-matrix the-matrix.c
	$(CC) $(CFLAGS) -O3 -o the-matrix.opt the-matrix.c

clean:
	$(RM) the-matrix the-matrix.opt perf.data perf.data.old
