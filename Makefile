ifeq ($(LUA_VERSION),)
LUA_VERSION=5.2
endif

ifeq ($(LUA_CPPFLAGS),)
LUA_CPPFLAGS=-I/usr/include/lua$(LUA_VERSION)
endif

ifeq ($(LUA_LIBS),)
LUA_LIBS=-llua$(LUA_VERSION)
endif

ifeq ($(LEXTLIB_LIBS),)
LEXTLIB_LIBS=-llextlib
endif

ifeq ($(ASSIMP_LIBS),)
ASSIMP_LIBS=-lassimp
endif

ifneq ($(DEBUG),)
EXTRA_CFLAGS+= -g -O0
endif

CFLAGS=-Wall -Werror -pedantic -std=c89 -fPIC -D_XOPEN_SOURCE=700 $(EXTRA_CFLAGS)
CPPFLAGS=$(LUA_CPPFLAGS) $(LEXTLIB_CPPFLAGS)
LDFLAGS=-Wl,--no-undefined $(LUA_LDFLAGS) $(LEXTLIB_LDFLAGS)
LIBS=$(LUA_LIBS) $(LEXTLIB_LIBS)

.PHONY: all
all: xio.so

xio.so: xio.o
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -o $@ $^ $(LIBS)

.PHONY: clean
clean:
	$(RM) *.o *.so
