#include <stdio.h>
#include <stdbool.h>

#include <sys/stat.h>

#include <SDL.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <luajit.h>

#include <physfs.h>

int main(int argc, char *argv[]) {
  if (argc != 2)
    exit(1);

  PHYSFS_init(NULL);

  struct stat info;
  if (stat(argv[1], &info) == 0) {
    if (S_ISDIR(info.st_mode)) {
    }
  } else {
    exit(1);
  }

  // Create Lua interpreter state.
  lua_State *vm = luaL_newstate();

  // Create game window.
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window =
      SDL_CreateWindow("RetroBoy", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 160, 144, 0);

  bool running = true;
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;
      }
    }
  }

  // Destroy game window.
  SDL_DestroyWindow(window);
  SDL_Quit();

  // Destroy Lua state.
  lua_close(vm);

  return 0;
}