#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <lua.h>
#include <lauxlib.h>


#include "lextlib/lextlib.h"
#include "lextlib/lextlib_lua52.h"


typedef luaL_Stream LStream;


/*
** When creating file handles, always creates a `closed' file handle
** before opening the actual file; so, if there is a memory error, the
** file is not left opened.
*/
static LStream *newprefile (lua_State *L) {
  LStream *p = (LStream *)lua_newuserdata(L, sizeof(LStream));
  p->closef = NULL;  /* mark file handle as 'closed' */
  luaL_setmetatable(L, LUA_FILEHANDLE);
  return p;
}

static LStream *newfile (lua_State *L) {
  LStream *p = newprefile(L);
  p->f = NULL;
  p->closef = NULL;
  return p;
}

static int fd_open(lua_State *L) {
	int fd = luaX_checkinteger(L, 1, "fd");
	const char *mode = luaX_checkstring(L, 2, "mode");

	LStream *p = newfile(L);

	int i = 0;
	/* check whether 'mode' matches '[rwa]%+?b?' */
	if (!(mode[i] != '\0' && strchr("rwa", mode[i++]) != NULL &&
			(mode[i] != '+' || ++i) &&  /* skip if char is '+' */
			(mode[i] != 'b' || ++i) &&  /* skip if char is 'b' */
			(mode[i] == '\0'))) {
		return luaL_error(L, "invalid mode " LUA_QS " (should match " LUA_QL("[rwa]%%+?b?") ")", mode);
	}

	p->f = fdopen(fd, mode);
	if (p->f == NULL) {
		const char *filename = lua_pushfstring(L, "fd:%d", fd);
		return luaL_fileresult(L, 0, filename);
	}

	return 1;
}


struct luaL_Reg fd_lib[] = {
	{ "open", fd_open },

	{ NULL, NULL }
};


int luaopen_fd(lua_State *L) {
	luaopen_io(L); /* initialise io metatable */
	lua_pop(L, 1);

	luaL_newlib(L, fd_lib);

	return 1;
}
