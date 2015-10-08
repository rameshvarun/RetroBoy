local FileSystem = class("FileSystem")

function FileSystem:initialize(path)
  self.prefix = path .. '/'
end

function FileSystem:exists(name)
  local f = io.open(self.prefix .. name, "r")
  if f~=nil then io.close(f) return true else return false end
end

-- isFile
-- isDir

function FileSystem:read(name)
  local f = io.open(self.prefix .. name, "rb")
  local data = f:read("*a")
  return data
end

-- enumerate

return FileSystem
