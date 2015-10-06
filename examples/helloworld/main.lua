-- retroboy.update is called every frame, with the time since the previous frame as an argument.
function retroboy.update(dt)
end

-- retroboy.draw is called every frame. After the call, the screen is updated with any changed pixels.
function retroboy.draw()
	retroboy.display.clear(0)
	retroboy.display.setPixel(0, 0, 1)
end