function euler(x,y,z)
	return Matrix():rotate(0,0,z) * Matrix():rotate(0,y,0) * Matrix():rotate(x,0,0)
end


function updateCamera(dt)	
	camera.fwd = Vector3(
		math.cos(camera.angle[2]) * math.sin(camera.angle[1]),
		math.sin(camera.angle[2]),
		math.cos(camera.angle[2]) * math.cos(camera.angle[1])
	)

	camera.rhs = Vector3(
		math.sin(camera.angle[1] + math.pi/2),
		0,
		math.cos(camera.angle[1] + math.pi/2)
    )

	if GetCursorMode() == CURSORMODE_CENTERED then
		mouseDelta = GetMouseDelta()
		camera.angle[1] = camera.angle[1] + (mouseDelta[1] * dt * SENSITIVITY)
		camera.angle[2] = camera.angle[2] - (mouseDelta[2] * dt * SENSITIVITY)
		
		camera.angle[2] = math.clamp(-1.52, 1.55, camera.angle[2])
	end

	lookAt = Matrix():lookAt(
		   camera.pos,
		   camera.pos+camera.fwd,
		   Vector3(0,1,0)
    )
end
