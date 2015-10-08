#include "device.hpp"

namespace retroboy {
int wrapped_DisplayClear(lua_State *L) {
  Device *device = (Device *)lua_touserdata(L, lua_upvalueindex(1));
  int color = luaL_checkinteger(L, 1);
  device->ClearDisplay(color);
  return 0;
}
int wrapped_DisplaySetPixel(lua_State *L) {
  Device *device = (Device *)lua_touserdata(L, lua_upvalueindex(1));
  int x = luaL_checkinteger(L, 1);
  int y = luaL_checkinteger(L, 2);
  int color = luaL_checkinteger(L, 3);
  device->SetPixel(x, y, color);
  return 0;
}
int GetLeft(lua_State *L) {
  auto keystate = SDL_GetKeyboardState(NULL);
  lua_pushboolean(L, keystate[SDL_SCANCODE_LEFT]);
  return 1;
}
int GetRight(lua_State *L) {
  auto keystate = SDL_GetKeyboardState(NULL);
  lua_pushboolean(L, keystate[SDL_SCANCODE_RIGHT]);
  return 1;
}
int GetUp(lua_State *L) {
  auto keystate = SDL_GetKeyboardState(NULL);
  lua_pushboolean(L, keystate[SDL_SCANCODE_UP]);
  return 1;
}
int GetDown(lua_State *L) {
  auto keystate = SDL_GetKeyboardState(NULL);
  lua_pushboolean(L, keystate[SDL_SCANCODE_DOWN]);
  return 1;
}

int GetStart(lua_State *L) {
  auto keystate = SDL_GetKeyboardState(NULL);
  lua_pushboolean(L, keystate[SDL_SCANCODE_RETURN]);
  return 1;
}
int GetSelect(lua_State *L) {
  auto keystate = SDL_GetKeyboardState(NULL);
  lua_pushboolean(L, keystate[SDL_SCANCODE_SPACE]);
  return 1;
}
int GetA(lua_State *L) {
  auto keystate = SDL_GetKeyboardState(NULL);
  lua_pushboolean(L, keystate[SDL_SCANCODE_Z]);
  return 1;
}
int GetB(lua_State *L) {
  auto keystate = SDL_GetKeyboardState(NULL);
  lua_pushboolean(L, keystate[SDL_SCANCODE_X]);
  return 1;
}

void Device::InstallDisplayAPI() {
  lua_getfield(vm_.get(), LUA_GLOBALSINDEX, "retroboy");

  // Create display table.
  {
    lua_createtable(vm_.get(), 0, 2);

    {
      lua_pushlightuserdata(vm_.get(), this);
      lua_pushcclosure(vm_.get(), wrapped_DisplayClear, 1);
      lua_setfield(vm_.get(), -2, "clear");
    }

    {
      lua_pushlightuserdata(vm_.get(), this);
      lua_pushcclosure(vm_.get(), wrapped_DisplaySetPixel, 1);
      lua_setfield(vm_.get(), -2, "setPixel");
    }

    lua_setfield(vm_.get(), -2, "display");
  }
  lua_pop(vm_.get(), 1);
}

void Device::InstallInputAPI() {
  lua_getfield(vm_.get(), LUA_GLOBALSINDEX, "retroboy");

  // Create input table.
  {
    lua_createtable(vm_.get(), 0, 8);

    lua_pushcfunction(vm_.get(), GetLeft);
    lua_setfield(vm_.get(), -2, "left");

    lua_pushcfunction(vm_.get(), GetRight);
    lua_setfield(vm_.get(), -2, "right");

    lua_pushcfunction(vm_.get(), GetUp);
    lua_setfield(vm_.get(), -2, "up");

    lua_pushcfunction(vm_.get(), GetDown);
    lua_setfield(vm_.get(), -2, "down");

    lua_pushcfunction(vm_.get(), GetStart);
    lua_setfield(vm_.get(), -2, "start");

    lua_pushcfunction(vm_.get(), GetSelect);
    lua_setfield(vm_.get(), -2, "select");

    lua_pushcfunction(vm_.get(), GetA);
    lua_setfield(vm_.get(), -2, "a");

    lua_pushcfunction(vm_.get(), GetB);
    lua_setfield(vm_.get(), -2, "b");

    lua_setfield(vm_.get(), -2, "input");
  }
  lua_pop(vm_.get(), 1);
}

Device::Device() {
  // Provide access to basic libaries.
  luaopen_math(vm_.get());
  luaopen_string(vm_.get());
  luaopen_table(vm_.get());
  luaopen_base(vm_.get());

  // Create retroboy table.
  lua_createtable(vm_.get(), 0, 2);
  lua_setglobal(vm_.get(), "retroboy");

  InstallDisplayAPI();
  InstallInputAPI();

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
        luaL_loadbuffer(vm_.get(), script_buffer, script_length, "main.lua") || lua_pcall(vm_.get(), 0, 0, 0);
    if (error) {
      throw std::runtime_error("Error loading script: " + std::string(lua_tostring(vm_.get(), -1)) + "\n");
    }
  } else {
    throw std::runtime_error("Missing main.lua file.\n");
  }
}

void Device::ClearDisplay(int color) {
  if (color > 3 || color < 0)
    throw std::invalid_argument("'color' must be between 0 and 3, inclusive.");
  SDL_FillRect(display_.get(), NULL, palette[color]);
}

void Device::SetPixel(int x, int y, int color) {
  if (x < 0 || y < 0 || x >= display_.get()->w || y >= display_.get()->h)
    throw std::invalid_argument("Pixel must be between (0,0), and (159, 143), inclusive.");
  if (color > 3 || color < 0)
    throw std::invalid_argument("'color' must be between 0 and 3, inclusive.");

  Uint32 *pixels = (Uint32 *)display_.get()->pixels;
  pixels[(y * display_->w) + x] = palette[color];
}

void Device::Step(double dt) {
  // Call draw function.
  lua_getfield(vm_.get(), LUA_GLOBALSINDEX, "retroboy");
  lua_getfield(vm_.get(), -1, "draw");
  if (lua_pcall(vm_.get(), 0, 0, 0)) {
    throw std::runtime_error("Error while drawing screen: %s\n" + std::string(lua_tostring(vm_.get(), -1)));
  }

  // Call update function.
  lua_getfield(vm_.get(), -1, "update");
  lua_remove(vm_.get(), -2);
  lua_pushnumber(vm_.get(), dt);
  if (lua_pcall(vm_.get(), 1, 0, 0)) {
    throw std::runtime_error("Error while updating: %s\n" + std::string(lua_tostring(vm_.get(), -1)));
  }
}

SDL_Surface* Device::GetDisplay() {
  return display_.get();
}

}
