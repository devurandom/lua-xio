ifeq ($(LUA_VERSION),)
LUA_VERSION=5.2
endif

ifneq ($(DEBUG),)
EXTRA_CFLAGS+= -g -O0
endif

LUA_CPPFLAGS=-I/usr/include/lua$(LUA_VERSION)

CFLAGS=-Wall -Werror -pedantic -std=c89 -fPIC -D_XOPEN_SOURCE=700 $(EXTRA_CFLAGS) $(LUA_CPPFLAGS)
LDFLAGS=-Wl,--no-undefined
LIBS=-llua$(LUA_VERSION)

.PHONY: all
all: xio.so

xio.so: xio.o lextlib/lextlib.o

xio.o: xio.c lextlib/lextlib.h lextlib/lextlib_global.h lextlib/lextlib_lua52.h

lextlib/lextlib.o:
	$(MAKE) -C lextlib

.PHONY: clean
clean:
	$(RM) *.o *.so

.SUFFIXES: .c .o .so

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

.o.so:
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -o $@ $^ $(LIBS)
