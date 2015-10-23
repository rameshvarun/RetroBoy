local InputDevice = class('InputDevice')

function InputDevice:initialize()
	error("InputDevice should not be directly instantiated.")
end

function InputDevice:isDown(key)
	error("Not implemented.")
end

return InputDevice
