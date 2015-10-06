#include <iostream>
#include <vector>

#include <SDL.h>
#include <lua.hpp>
#include <physfs.h>
#include <json/json.h>

using namespace std;

#define PANIC(...)                                                                                 \
  {                                                                                                \
    fprintf(stderr, __VA_ARGS__);                                                                  \
    exit(1);                                                                                       \
  }

SDL_Surface *display;
vector<Uint32> palette{0x0f380f, 0x306230, 0x8bac0f, 0x9bbc0f};

int display_clear(lua_State *L) {
  int color = luaL_checkinteger(L, 1);
  if (color > 3 || color < 0)
    luaL_error(L, "Invalid color: %d", color);
  SDL_FillRect(display, NULL, palette[color]);
  return 0;
}
int display_setPixel(lua_State *L) {
  int x = luaL_checkinteger(L, 1);
  int y = luaL_checkinteger(L, 2);
  int color = luaL_checkinteger(L, 3);

  if (x < 0 || y < 0 || x >= display->w || y >= display->h)
    luaL_error(L, "Invalid pixel: (%d, %d)", x, y);
  if (color > 3 || color < 0)
    luaL_error(L, "Invalid color: %d", color);

  Uint32 *pixels = (Uint32 *)display->pixels;
  pixels[(y * display->w) + x] = palette[color];

  return 0;
}

int vec_main(vector<string> argv) {
  if (argv.size() != 2)
    PANIC("Missing an argument.\n");

  // Initialize filesystem.
  PHYSFS_init(NULL);
  if (PHYSFS_mount(argv[1].c_str(), NULL, 1) == 0) {
    std::cerr << "Failed to mount '" << argv[1] << "' as filesystem." << std::endl;
    exit(1);
  }

  // Parse configuration file.
  if (PHYSFS_exists("conf.json")) {
  } else {
    PANIC("Missing conf.json file.\n");
  }

  // Create game window.
  SDL_Init(SDL_INIT_VIDEO);
  auto window = SDL_CreateWindow("RetroBoy", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                 160 * 2, 144 * 2, SDL_WINDOW_SHOWN);
  auto window_surface = SDL_GetWindowSurface(window);

  if (window == NULL) {
    PANIC("Could not create window. Error: %s.\n", SDL_GetError());
    exit(1);
  }

  display = SDL_CreateRGBSurface(0, 160, 144, 32, 0, 0, 0, 0);

  // Create Lua interpreter state.
  auto vm = luaL_newstate();
  luaopen_math(vm);
  luaopen_string(vm);
  luaopen_table(vm);
  luaopen_base(vm);

  // Create retroboy table.
  lua_createtable(vm, 0, 2);
  {
    // Create display table.
    lua_createtable(vm, 0, 2);

    lua_pushcfunction(vm, display_clear);
    lua_setfield(vm, -2, "clear");

    lua_pushcfunction(vm, display_setPixel);
    lua_setfield(vm, -2, "setPixel");

    lua_setfield(vm, -2, "display");
  }
  lua_setglobal(vm, "retroboy");

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
  auto lastTime = SDL_GetTicks();
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;
      }
    }

    // Call draw function.
    lua_getfield(vm, LUA_GLOBALSINDEX, "retroboy");
    lua_getfield(vm, -1, "draw");
    lua_remove(vm, -2);
    if (lua_pcall(vm, 0, 0, 0)) {
      PANIC("Error drawing screen: %s\n", lua_tostring(vm, -1));
    }

    lua_getfield(vm, LUA_GLOBALSINDEX, "retroboy");
    lua_getfield(vm, -1, "update");
    lua_remove(vm, -2);
    lua_pushnumber(vm, (SDL_GetTicks() - lastTime) / 1000.0);
    if (lua_pcall(vm, 1, 0, 0)) {
      PANIC("Error updating: %s\n", lua_tostring(vm, -1));
    }
    lastTime = SDL_GetTicks();

    SDL_BlitScaled(display, NULL, window_surface, NULL);

    // Update window.
    SDL_UpdateWindowSurface(window);
  }

  // Destroy game window.
  SDL_FreeSurface(display);
  SDL_DestroyWindow(window);
  SDL_Quit();

  // Destroy Lua state.
  lua_close(vm);

  // De-initialize PHYSFS.
  PHYSFS_deinit();

  return 0;
}

int main(int argc, char *argv[]) {
  vector<string> args;
  for (int i = 0; i < argc; ++i)
    args.push_back(string(argv[i]));
  vec_main(args);
}
