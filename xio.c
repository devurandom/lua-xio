#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <lua.h>
#include <lauxlib.h>

#include <lextlib.h>
#include <lextlib_lua52.h>


int luaopen_io (lua_State *L);

typedef luaL_Stream LStream;


#define tolstream(L)	((LStream *)luaL_checkudata(L, 1, LUA_FILEHANDLE))

/*
** function to (not) close the standard files stdin, stdout, and stderr
*/
static int io_noclose (lua_State *L) {
  LStream *p = tolstream(L);
  p->closef = &io_noclose;  /* keep file opened */
  lua_pushnil(L);
  lua_pushliteral(L, "cannot close standard file");
  return 2;
}

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
  p->closef = &io_noclose;
  return p;
}

static int xio_fdopen(lua_State *L) {
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

static int xio_mkfifo(lua_State *L) {
	const char *filename = luaX_checkstring(L, 1, "filename");
	const char *modestr = luaX_checkstring(L, 2, "mode");

	int err = 0;
	mode_t mode = 0;
	if (strchr(modestr, 'r') != NULL) {
		mode |= S_IRUSR;
	}
	if (strchr(modestr, 'w') != NULL) {
		mode |= S_IWUSR;
	}
	if (strchr(modestr, 'x') != NULL) {
		mode |= S_IXUSR;
	}

	err = mkfifo(filename, mode);
	return luaL_fileresult(L, err == 0, filename);
}


struct luaL_Reg xio_lib[] = {
	{ "fdopen", xio_fdopen },
	{ "mkfifo", xio_mkfifo },

	{ NULL, NULL }
};


int luaopen_xio(lua_State *L) {
	luaL_requiref(L, "io", luaopen_io, 0); /* initialise io metatable */ /* [-0,+1,e] */
	lua_pop(L, 1);

	luaL_newlib(L, xio_lib);

	return 1;
}
