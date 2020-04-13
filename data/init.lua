spinner = 0.0
quad = nil
cube = nil
rot = nil
lookAt = nil
whiteTex = Texture("mafiahub.bmp")

dofile("another.lua")

SPEED = 15.0
SENSITIVITY = 0.15

camera = {
	pos = Vector3(0,0,0),
	dir = Vector3(0.7,-0.3,0),
	angle = {0,0}
}

function updateCamera(dt)
	lookAt = Matrix():lookAt(
		   camera.pos,
		   camera.pos+camera.dir,
		   Vector3(0,1,0)
    )

	mouseDelta = GetMouseDelta()
	
	camera.dir = Vector3(
		math.cos(camera.angle[2]) * math.sin(camera.angle[1]),
		math.sin(camera.angle[2]),
		math.cos(camera.angle[2]) * math.cos(camera.angle[1])
	)

	if GetCursorMode() == 1 then
		camera.angle[1] = camera.angle[1] + (mouseDelta[1]) * dt * SENSITIVITY
		camera.angle[2] = camera.angle[2] - (mouseDelta[2]) * dt * SENSITIVITY
	end
end


function _init()
	CameraPerspective(45)
	-- CameraOrthographic(5,5)
	-- updateCamera(0)

	-- ShowCursor(false)
	
	quad = Mesh()
	quad:addVertex(Vertex( -1.0, 1.0, -1.0, 0, 0, Color(255, 255, 255)))
	quad:addVertex(Vertex( 1.0, 1.0, -1.0, -1.0, 0.0, Color(255, 255, 255)))
	quad:addVertex(Vertex( -1.0, -1.0, -1.0, 0.0, -1.0, Color(255, 255, 255)))
	quad:addVertex(Vertex( 1.0, -1.0, -1.0, -1.0, -1.0, Color(255, 0, 0)))
	
	quad:addTriangle(0,1,2)
	quad:addTriangle(2,1,3)
	
	quad:setTexture(0, whiteTex)
	
	quad:build()

	cube = MeshGroup()
	cube:addMesh(quad, Matrix())
	cube:addMesh(quad, Matrix():rotate(0,math.rad(90),0))
	cube:addMesh(quad, Matrix():rotate(0,math.rad(-90),0))
	cube:addMesh(quad, Matrix():rotate(0,math.rad(180),0))
	cube:addMesh(quad, Matrix():rotate(math.rad(90),0,0))
	cube:addMesh(quad, Matrix():rotate(math.rad(-90),0,0))
end

function _destroy()

end

function _update(dt)
	spinner = spinner + 1.5*dt
	rot = euler(spinner, math.sin(spinner), 0)

	-- F2
	if GetKeyDown(0x71) then
		ShowCursor(not IsCursorVisible())
		SetCursorMode(1-GetCursorMode())
		camera.mouse = GetMouseXY()
	end 

	-- ESC
	if GetKeyDown(0x1B) then
		ExitGame()	
	end

	if GetKey(0x57) then
		camera.pos = camera.pos + (camera.dir * dt * SPEED)
	end

	if GetKey(0x53) then
		camera.pos = camera.pos - (camera.dir * dt * SPEED)
	end

	if GetKey(0x41) then
		camera.pos = Vector3(
				  camera.pos:x() - camera.dir:z() * dt * SPEED,
				  camera.pos:y(),
				  camera.pos:z() + camera.dir:x() * dt * SPEED
        )
	end

	if GetKey(0x44) then
		camera.pos = Vector3(
				  camera.pos:x() + camera.dir:z() * dt * SPEED,
				  camera.pos:y(),
				  camera.pos:z() - camera.dir:x() * dt * SPEED
        )
	end

	updateCamera(dt)
end

function _render()
	ClearScene(0, 40, 100)
	
	lookAt:bind(VIEW)
	
	for i=0, 15, 1
	do
		for j=0, 15, 1
		do
			w = (rot * Matrix():translate(i*2, 0, j*2))
			cube:draw(w)
		end
	end
end
