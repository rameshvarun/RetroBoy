-- retroboy.update is called every frame, with the time since the previous frame as an argument.
function retroboy.update(dt) end

-- retroboy.draw is called every frame. After the call, the screen is updated with any changed pixels.
function retroboy.draw()
end

function mandelbrot()
	for px=0,159 do
		for py=0,143 do
			local x0 = (px/160.0)*3.5 - 2.5
			local y0 = (py/144)*2.0 - 1.0

			local x = 0
			local y = 0
			local iteration = 0
			local max_iteration = 1000
			
			while x*x + y*y < 4 and iteration < max_iteration do
				local xtemp = x*x - y*y + x0
				y = 2*x*y + y0
				x = xtemp
				iteration = iteration + 1
			end

			local value = (iteration - 1)/max_iteration
			local color = math.floor( math.pow(value, 0.3)*4.0 )
			retroboy.display.setPixel(px, py, color)
		end
	end
end

-- Clear the display
retroboy.display.clear(0)
mandelbrot()
