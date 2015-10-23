local DirFS = class("DirFS")
local OS = love.system.getOS()

function DirFS:initialize(path)
  self.prefix = path .. '/'
end

function DirFS:exists(name)
  local f = io.open(self.prefix .. name, "r")
  if f~=nil then io.close(f) return true else return false end
end

function DirFS:read(name)
  local f = io.open(self.prefix .. name, "rb")
  local data = f:read("*a")
  f:close()
  
  return data
end

function DirFS:enumerate(path)
	local files = {}

	if OS == "OS X" or OS == "Linux" then
		for file in io.popen('ls ' .. self.prefix .. path):lines() do
			table.insert(files, file)
		end
	else
		error("DirFS:enumerate not implemented for OS: " + OS)
	end

	return files
end

function DirFS:isFile(path)
	if OS == "OS X" or OS == "Linux" then
		return os.execute('test -f ' .. self.prefix .. path) == 0
	else
		error("DirFS:isFile not implemented for OS: " + OS)
	end
end

function DirFS:isDir(path)
	if OS == "OS X" or OS == "Linux" then
		return os.execute('test -d ' .. self.prefix .. path) == 0
	else
		error("DirFS:isDir not implemented for OS: " + OS)
	end
end

return DirFS
