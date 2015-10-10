class = require "vendor.middleclass"
_ = require "vendor.underscore"
util = require "util"

local Device = require "device"
local FileSystem = require "filesystem"

local KeyboardDevice = require "input.keyboarddevice"
local JoystickDevice = require "input.joystickdevice"

function love.load(arg)
  if arg[2] == nil then error("No rom specified.") end
  device = Device(FileSystem(arg[2]), KeyboardDevice())
end

function love.draw()
  device:draw()
  love.graphics.setCanvas()
  love.graphics.setColor(255, 255, 255, 255)
  love.graphics.draw(device:getDisplay(), 0, 0, 0, 4, 4)
end

function love.update(dt)
  device:update(dt)
end
