SPEED = 15.0
SENSITIVITY = 0.15

lookAt = nil

camera = {
	pos = Vector3(0,6,-48),
	fwd = Vector3(),
	fwdl = Vector3(),
	rhs = Vector3(),
	vel = Vector3(),
	angle = {0.0,0.0},
	heldControls = false
}

function updateCamera(dt)
	handleInput(dt)

	if GetCursorMode() == CURSORMODE_CENTERED then
		mouseDelta = GetMouseDelta()
		camera.angle[1] = camera.angle[1] + (mouseDelta[1] * dt * SENSITIVITY)
		camera.angle[2] = camera.angle[2] - (mouseDelta[2] * dt * SENSITIVITY)
		
		-- camera.angle[2] = math.clamp(-1.52, 1.55, camera.angle[2])
	end

	lookAt = Matrix():translate(camera.pos*-1)
					  :rotate(-camera.angle[1], 0, 0)
					  :rotate(0, camera.angle[2], 0)

	camera.fwd = lookAt:col(3)
	camera.rhs = lookAt:col(1)

	camera.pos = camera.pos + camera.vel
	camera.vel = camera.vel + camera.vel:neg()*0.10
end

function handleInput(dt)
	local vel = Vector3()

	if GetKey("w") then
		vel = vel + Vector3(camera.fwd * dt * SPEED)
	end

	if GetKey("s") then
		vel = vel + Vector3(camera.fwd * dt * SPEED):neg()
	end

	if GetKey("a") then
		vel = vel + Vector3(camera.rhs * dt * SPEED):neg()
	end

	if GetKey("d") then
		vel = vel + Vector3(camera.rhs * dt * SPEED)
	end

	if GetKey(KEY_SHIFT) then
		vel = vel * 8
	end

	if vel:mag() ~= 0 then
		camera.vel = camera.vel + (Vector3(vel) - camera.vel)*0.10
		camera.heldControls = true
	end
end

function euler(x,y,z)
	return Matrix():rotate(0,0,z) * Matrix():rotate(0,y,0) * Matrix():rotate(x,0,0)
end

function lerp( a,b,t )
	return a + (b - a)*t
end
