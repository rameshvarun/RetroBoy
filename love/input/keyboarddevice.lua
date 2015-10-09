local InputDevice = require "input.inputdevice"
local KeyboardDevice = class('KeyboardDevice', InputDevice)

function KeyboardDevice:initialize()
end

local mapping = {
	up = 'up',
	down = 'down',
	left = 'left',
	right = 'right',
	a = 'z',
	b = 'x',
	start = 'return',
	select = 'lshift',
	guide = 'escape'
}

function KeyboardDevice:isDown(key)	
	return love.keyboard.isDown(mapping[key])
end

return KeyboardDevice