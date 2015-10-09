local InputDevice = require "input.inputdevice"
local JoystickDevice = class('JoystickDevice', InputDevice)

function JoystickDevice:initialize(joystick)
	self.joystick = joystick
	self.useGamepadMappings = joystick:isGamepad()
end

local gamepadMapping = {
	up = 'dpup',
	down = 'dpdown',
	left = 'dpleft',
	right = 'dpright',
	a = 'a',
	b = 'b',
	start = 'start',
	select = 'back',
	guide = 'guide'
}

function JoystickDevice:isDown(key)
	if self.useGamepadMappings then
		return self.joystick:isGamepadDown(gamepadMapping[key])
	end
end

return JoystickDevice