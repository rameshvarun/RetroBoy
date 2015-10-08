#pragma once

#include <memory>
#include <vector>
#include <string>

#include <SDL.h>
#include <lua.hpp>
#include <physfs.h>

namespace retroboy {
  // These wrapper functions convert the arguments, as well as extract the
  // current Device* pointer from the closure.
  int display_clear(lua_State* L);
  int display_setPixel(lua_State* L);

  class Device {
  private:
    std::vector<uint32_t> palette{0x0f380f, 0x306230, 0x8bac0f, 0x9bbc0f};

    std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)> display_ {
      SDL_CreateRGBSurface(0, 160, 144, 32, 0, 0, 0, 0), SDL_FreeSurface};
    std::unique_ptr<lua_State, void(*)(lua_State*)> vm_ {luaL_newstate(), lua_close};

    void InstallDisplayAPI();

  public:
    Device();
    void SetPixel(int x, int y, int color);
    void ClearDisplay(int color);
    void Step(double dt);
    SDL_Surface* GetDisplay();
  };
}
