ifeq ($(LUA_VERSION),)
LUA_VERSION=5.2
endif

CFLAGS=-Wall -Werror -std=c89 -pedantic -g -fPIC -I/usr/include/lua$(LUA_VERSION) -D_XOPEN_SOURCE=700
C99FLAGS_RELAXED=-std=c99 -fPIC -I/usr/include/lua$(LUA_VERSION) -D_XOPEN_SOURCE=700
LDFLAGS=-Wl,--no-undefined
LIBS=-llua$(LUA_VERSION)

all: xio.so

xio.o: xio.c lextlib/lextlib_lua52.h

xio.so: xio.o lextlib.o

lextlib.o: lextlib/lextlib.c
	$(CC) $(C99FLAGS_RELAXED) -o $@ -c $<

clean:
	$(RM) *.so *.o

.SUFFIXES: .c .o .so

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

.o.so:
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -o $@ $^ $(LIBS)
