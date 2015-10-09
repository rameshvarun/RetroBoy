local FileSystem = class("FileSystem")
local prettyprint = require "prettyprint"

function FileSystem:initialize(path)
  self.prefix = path .. '/'
end

function FileSystem:exists(name)
  local f = io.open(self.prefix .. name, "r")
  if f~=nil then io.close(f) return true else return false end
end

function FileSystem:read(name)
  local f = io.open(self.prefix .. name, "rb")
  local data = f:read("*a")
  f:close()
  
  return data
end

function FileSystem:enumerate(path)
	-- TODO: Make this more portable.
	local files = {}
	for file in io.popen('ls ' .. self.prefix .. path):lines() do
		table.insert(files, file)
	end
	return files
end

function FileSystem:isFile(path)
	return os.execute('test -f ' .. self.prefix .. path) == 0
end

function FileSystem:isDir(path)
	return os.execute('test -d ' .. self.prefix .. path) == 0
end


return FileSystem
