local Device = class('Device')
local json = require "vendor.dkjson"

function Device:initialize(fs, input)
  -- Default color palette.
  self.palette = {{15, 56, 15}, {48, 98, 48}, {139, 172, 15}, {155, 188, 15}}

  self.display = love.graphics.newCanvas(160, 144, 'normal', 0)
  self.display:setFilter( "nearest", "nearest", 0 )

  self.fs = fs
  self.input = input
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
    getfenv = getfenv, -- NOTE: Can be used to break out of the sandbox.
    getmetatable = getmetatable,
    ipairs = ipairs,

    -- TODO: Wrap load
    -- TODO: Wrap loadfile
    -- TODO: Wrap loadstring
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

    -- Implemented based off of the description in http://www.lua.org/manual/5.2/manual.html#6.3
    require = function (modname)
	-- If the module has already been loaded, simply return it.
    	if self.env.package.loaded[modname] ~= nil then
		return self.env.package.loaded[modname]
	end

	-- Try all of the searchers, looking for one that returns a loader.
	for _, searcher in ipairs(self.env.package.searchers) do
		-- Try this searcher, which can return a loader function, along
		-- with some extra data.
		local loader, data = searcher(modname)
		if type(loader) == "function" then
			local module = loader(modname, data)
			self.env.package.loaded[modname] = module
			return module
		end
	end

	error("Could not find a loader for module " .. modname)
    end,

    package = {
	    config = "/\n;\n?\n!\n-\b",
	    path = "/?.lua",
	    loaded = {}, -- Loaders for modules are cached here.
	    preload = {},
	    searchers = {
		-- Preload searcher.
	    	function (modname)
			return self.env.package.preload[modname]
		end,
		-- Temporary searcher that starts from FS root.
		-- TODO: Search in the path, thus allowing code that modifies the search path.
		function (modname)
			modname = modname:gsub("%.", "/")
			if modname:sub(-4) ~= ".lua" then
				modname = modname .. ".lua"
			end
			
			if self.fs:exists(modname) then
				local loader = function(modname, filename)
					local chunk = loadstring(self.fs:read(filename), filename)
 					setfenv(chunk, self.env)
					return chunk()
				end
				return loader, modname
			end
		end
	    },
	    searchpath = function(name, path, sep, rep)
		    error("package.searchpath not yet implemented.")
	    end
    }
  }

  -- Make sure that _G within the scope refers to the sandboxed scope.
  self.env["_G"] = self.env

  -- Global retroboy table.
  self.env.retroboy = {}

  -- Display API.
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

  -- Filesystem API.
  self.env.retroboy.filesystem = {
    exists = function(path) return fs:exists(path) end,
    read = function(path) return fs:read(path) end,
    enumerate = function(path) return fs:enumerate(path) end,
    isFile = function(path) return fs:isFile(path) end,
    isDir = function(path) return fs:isDir(path) end
  }

  -- Input API.
  self.env.retroboy.input = {
    left = function() return self.input:isDown("left") end,
    right = function() return self.input:isDown("right") end,
    up = function() return self.input:isDown("up") end,
    down = function() return self.input:isDown("down") end,

    a = function() return self.input:isDown("a") end,
    b = function() return self.input:isDown("b") end,

    start = function() return self.input:isDown("start") end,
    select = function() return self.input:isDown("select") end
  }

  -- Ensure that there is both a conf.json and main.lua.
  if not self.fs:exists('conf.json') then error('No conf.json') end
  if not self.fs:exists('main.lua') then error('No main.lua') end

  -- Load in configs.
  self.config = json.decode(self.fs:read('conf.json'))
  self.palette = _.map(self.config.palette, util.hex2rgb)

  -- Try to load the main.lua chunk.
  local chunk, err = loadstring(self.fs:read('main.lua'), 'main.lua')
  if not chunk then
    error("Error reading main.lua: " .. err)
  end
  setfenv(chunk, self.env)

  -- Try to run the main.lua chunk. This should populate the necessary
  -- callbacks.
  local status, err = pcall(chunk)
  if not status then
    error(err)
  end
end

function Device:getDisplay()
  return self.display
end

-- Start the device draw cycle.
function Device:draw()
  if self.env.retroboy.draw ~= nil then
    self.env.retroboy.draw()
  else
    error("No draw callback defined.")
  end
end

-- Start the device update cycle.
function Device:update(dt)
  if self.env.retroboy.update ~= nil then
    self.env.retroboy.update(dt)
  else
    error("No draw callback defined.")
  end
end

return Device
