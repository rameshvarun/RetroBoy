# API Description
The RetroBoy environment consists of a couple of APIs, which are as minimal as possible, in order to facilitate porting.

## Callbacks
These callbacks should be defined by the game, and are called by the platform.
### `retroboy.draw()`
Called every frame. In this callback, you should update the dispaly pixels as appropriate. After the callback is done, all of the updated pixels are pushed at once.

### `retroboy.update(dt)`
Called every frame, with the time since the last frame as an argument.

### `retroboy.debug.filechanged(name)`
If the player is in debug mode, it may be able to notify the game when a data file has changed. The game can the use this for things like hotswapping code and images.

## Display API
### `retroboy.display.clear(color)`
Clears the display to the specified color. `color` is an integer from 0-3 (inclusive), which acts as an index into the palette.

### `retroboy.display.setPixel(x, y, color)`
Sets the pixel at position (x, y) to the specified color. `color` is an integer from 0-3 (inclusive), which acts as an index into the palette. `x` and `y` should be within the display bounds.

## Filesystem API
RetroBoy games can access their game data directory through a read-only filesystem.
### `retroboy.filesystem.exists(path)`
Returns true if the file exists.

### `retroboy.filesystem.read(path)`
Reads the file into a string. In Lua, strings also act as byte arrays, thus this can also be used to read binary files.

### `retroboy.filesystem.isFile(path)`
Returns true if the path points to a file.

### `retroboy.filesystem.isDir(path)`
Returns true if the path points to a directory.

### `retroboy.filesystem.enumerate(path)`
Returns the names of all of the files and subdirectories under `path`.

## Input API
RetroBoy games have access to the same buttons as a GameBoy. The players can decide how to map physical controls to virtual controls.

### `retroboy.input.up()`
Checks if the DPad Up is pressed.

### `retroboy.input.down()`
Checks if the DPad Down is pressed.

### `retroboy.input.left()`
Checks if the DPad Left is pressed.

### `retroboy.input.right()`
Checks if the DPad Right is pressed.

### `retroboy.input.a()`
Checks if the A button is pressed.

### `retroboy.input.b()`
Checks if the B button is pressed.

### `retroboy.input.start()`
Checks if the Start button is pressed.

### `retroboy.input.select()`
Checks if the Select button is pressed.

## Link Cable API
TBD: Allow multiplayer games without a player cap.

## Audio API
TBD: Should replicated the functionality of the GameBoy sound chip.

## Save Data API
### `retroboy.save.set(key, value)`
### `retroboy.save.get(key)`
TBD: `retroboy.save.gobal.set(key, value)`
TBD: `retroboy.save.global.get(key)`
