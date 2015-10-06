#include <stdio.h>
#include <stdbool.h>

#include <sys/stat.h>

#include <SDL.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <luajit.h>

#include <physfs.h>

#include <jansson.h>

#define PANIC(...)                                                                                 \
  {                                                                                                \
    fprintf(stderr, __VA_ARGS__);                                                                  \
    exit(1);                                                                                       \
  }

int main(int argc, char *argv[]) {
  if (argc != 2)
    PANIC("Missing an argument.\n");

  // Initialize filesystem.
  PHYSFS_init(NULL);
  if (PHYSFS_mount(argv[1], NULL, 1) == 0) {
    PANIC("Failed to mount '%s' as filesystem.\n", argv[1]);
  }

  // Parse configuration file.
  if (PHYSFS_exists("conf.json")) {
  } else {
    PANIC("Missing conf.json file.\n");
  }

  // Create game window.
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window = SDL_CreateWindow("RetroBoy", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, 160, 144, SDL_WINDOW_SHOWN);
  if (window == NULL) {
    PANIC("Could not create window. Error: %s.\n", SDL_GetError());
  }

  SDL_Surface *screen = SDL_CreateRGBSurface(0, 160, 144, 32, 0, 0, 0, 0);

  // Create Lua interpreter state.
  lua_State *vm = luaL_newstate();
  luaopen_math(vm);
  luaopen_string(vm);

  // Run main.lua file
  if (PHYSFS_exists("main.lua")) {
    PHYSFS_file *script = PHYSFS_openRead("main.lua");
    PHYSFS_sint64 script_length = PHYSFS_fileLength(script);

    // Read file data into buffer.
    char script_buffer[script_length];
    PHYSFS_read(script, script_buffer, 1, script_length);
    PHYSFS_close(script);

    // Execute script contents.
    int error =
        luaL_loadbuffer(vm, script_buffer, script_length, "main.lua") || lua_pcall(vm, 0, 0, 0);
    if (error) {
      PANIC("Error loading script: %s\n", lua_tostring(vm, -1));
    }
  } else {
    PANIC("Missing main.lua file.\n");
  }

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
  SDL_FreeSurface(screen);
  SDL_DestroyWindow(window);
  SDL_Quit();

  // Destroy Lua state.
  lua_close(vm);

  // De-initialize PHYSFS.
  PHYSFS_deinit();

  return 0;
}