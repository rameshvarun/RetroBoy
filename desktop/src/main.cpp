#include <iostream>
#include <vector>

#include <SDL.h>
#include <lua.hpp>
#include <physfs.h>
#include <json/json.h>
#include "device.hpp"

int vec_main(std::vector<std::string> argv) {
  if (argv.size() != 2)
    throw std::invalid_argument("Missing an directory or rom file as argument.\n");

  // Initialize filesystem.
  PHYSFS_init(NULL);
  if (PHYSFS_mount(argv[1].c_str(), NULL, 1) == 0) {
    throw std::runtime_error("Failed to mount filesystem.");
  }

  // Parse configuration file.
  if (PHYSFS_exists("conf.json")) {
  } else {
  }

  // Create game window.
  SDL_Init(SDL_INIT_VIDEO);

  auto window = SDL_CreateWindow("RetroBoy", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                 160 * 2, 144 * 2, SDL_WINDOW_SHOWN);
  auto window_surface = SDL_GetWindowSurface(window);

  if (window == NULL) {
    throw std::runtime_error("Could not create SDL window. Error: " + std::string(SDL_GetError()) + "\n");
  }

  // Create the virtual device.
  retroboy::Device device;

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

    device.Step((SDL_GetTicks() - lastTime) / 1000.0);
    lastTime = SDL_GetTicks();

    SDL_BlitScaled(device.GetDisplay(), NULL, window_surface, NULL);

    // Update window.
    SDL_UpdateWindowSurface(window);
  }

  // Destroy game window.
  SDL_DestroyWindow(window);
  SDL_Quit();

  // De-initialize PHYSFS.
  PHYSFS_deinit();

  return 0;
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args;
  for (int i = 0; i < argc; ++i)
    args.push_back(std::string(argv[i]));
  vec_main(args);
}
