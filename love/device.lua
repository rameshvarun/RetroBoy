local Device = class('Device')

function Device:initialize(fs)
  self.palette = {{15, 56, 15}, {48, 98, 48}, {139, 172, 15}, {155, 188, 15}}

  self.display = love.graphics.newCanvas(160, 144, 'normal', 0)
  self.display:setFilter( "nearest", "nearest", 0 )

  self.fs = fs
  self.env = {
    math = math,
    string = string,
    table = table,
    -- Coroutines not allowed
    -- IO not allowed
    -- OS not allowed
    -- GC manipulation not allowed

    assert = assert,

    -- TODO: Wrap dofile

    error = error,
    getfenv = getfenv,
    getmetatable = getmetatable,
    ipairs = ipairs,

    load = load,
    -- TODO: Wrap loadfile
    loadstring = loadstring,
    next = next,

    pairs = pairs,
    pcall = pcall,

    print = print,

    rawequal = rawequal,
    rawget = rawget,
    rawset = rawset,

    select = select,
    setfenv = setfenv,
    setmetatable = setmetatable,
    tonumber = tonumber,
    tostring = tostring,
    type = type,
    unpack = unpack,
    xpcall = xpcall,

    -- Figure out require
  }

  self.env.retroboy = {}

  self.env.retroboy.display = {
    clear = function(color)
      self.display:clear(self.palette[color + 1])
    end,
    setPixel = function(x, y, color)
      love.graphics.setCanvas(self.display)
      love.graphics.setColor(self.palette[color + 1])

      love.graphics.setPointStyle("rough")
      love.graphics.point(x + 0.5, y + 0.5)

      love.graphics.setColor(255, 255, 255, 255)
      love.graphics.setCanvas()
    end
  }

  self.env.retroboy.filesystem = {
    exists = function(name)
      return fs:exists(name)
    end,
    read = function(name)
      return fs:read(name)
    end
  }

  self.env.retroboy.input = {
  }

  if not self.fs:exists('conf.json') then error('No conf.json') end
  if not self.fs:exists('main.lua') then error('No main.lua') end

  local chunk = loadstring(self.fs:read('main.lua'), 'main.lua')
  setfenv(chunk, self.env)

  local status, err = pcall(chunk)
  if not status then
    error(err)
  end

end

function Device:getDisplay()
  return self.display
end

function Device:draw()
  self.env.retroboy.draw()
end

function Device:update(dt)
  self.env.retroboy.update(dt)
end

return Device
