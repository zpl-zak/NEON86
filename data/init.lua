spinner = 0.0
quad = nil
cube = nil
rot = nil
lookAt = nil
twodeeTriangle = nil
whiteTex = Texture("mafiahub.bmp")

dofile("another.lua")

SPEED = 15.0
SENSITIVITY = 0.15

camera = {
	pos = Vector3(0,0,0),
	fwd = Vector3(0,0,0),
	rhs = Vector3(0,0,0),
	angle = {math.deg(90),0}
}

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
	end

	lookAt = Matrix():lookAt(
		   camera.pos,
		   camera.pos+camera.fwd,
		   Vector3(0,1,0)
    )
end

function _init()
	ShowCursor(false)
	SetCursorMode(CURSORMODE_CENTERED)
	
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

	twodeeTriangle = Mesh()
	twodeeTriangle:addVertex(Vertex( -1.0, 1.0, -1.0, 0, 0, Color(255, 255, 255)))
	twodeeTriangle:addVertex(Vertex( 1.0, 1.0, -1.0, -1.0, 0.0, Color(255, 255, 255)))
	twodeeTriangle:addVertex(Vertex( -1.0, -1.0, -1.0, 0.0, -1.0, Color(255, 255, 255)))

	twodeeTriangle:addTriangle(0,1,2)
	twodeeTriangle:build()
end

function _destroy()

end

function _update(dt)
	spinner = spinner + 1.5*dt
	rot = euler(spinner, math.sin(spinner), 0)

	if GetKeyDown(KEY_F2) then
		ShowCursor(not IsCursorVisible())
		SetCursorMode(1-GetCursorMode())
	end 

	if GetKeyDown(KEY_ESCAPE) then
		ExitGame()	
	end

	if GetKey("w") then
		camera.pos = camera.pos + (camera.fwd * dt * SPEED)
	end

	if GetKey("s") then
		camera.pos = camera.pos - (camera.fwd * dt * SPEED)
	end

	if GetKey("a") then
		camera.pos = camera.pos - (camera.rhs * dt * SPEED)
	end

	if GetKey("d") then
		camera.pos = camera.pos + (camera.rhs * dt * SPEED)
	end

	updateCamera(dt)
end

function _render()
	ClearScene(0, 40, 100)
	CameraPerspective(70)
	
	lookAt:bind(VIEW)
	
	for i=0, 15, 1
	do
		for j=0, 15, 1
		do
			w = (rot * Matrix():translate(-i*2, 0, j*2))
			cube:draw(w)
		end
	end
end

function _render2d( )
	-- TODO UI SYSTEM
	--[[
		   MOCKUP:

		   UIBegin()
			UIQuad(0,0,0.5,0.5, Color(255,0,0))
			UIText(0,0,12,"Hello World!")
		   UIEnd()
    ]]
end
