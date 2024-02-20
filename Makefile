CC            = gcc
CFLAGS        = -O0 -g -fsanitize=address -fno-omit-frame-pointer -I./include
DEST          = /usr/local/bin
LDFLAGS       = -L./lib
LIBS          = -Wl,-rpath,./lib -lcassandra
OBJS          = sample.c
PROGRAM       = sample

all:            $(PROGRAM)

$(PROGRAM):     $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $(PROGRAM)

clean:;         rm -f *.o *~ $(PROGRAM)
