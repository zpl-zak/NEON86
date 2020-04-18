spinner = 0.0
quad = nil
cube = nil
rot = nil
lookAt = nil
whiteTex = Texture("mafiahub.bmp")

whiteTex:setSamplerState(SAMPLERSTATE_MAGFILTER, TEXF_POINT)
whiteTex:setSamplerState(SAMPLERSTATE_MINFILTER, TEXF_POINT)
whiteTex:setSamplerState(SAMPLERSTATE_MIPFILTER, TEXF_POINT)

dofile("another.lua")

SPEED = 15.0
SENSITIVITY = 0.15

camera = {
	pos = Vector3(0,0,0),
	fwd = Vector3(0,0,0),
	rhs = Vector3(0,0,0),
	angle = {0.0,0.0}
}

starfield = {}
starradius = 64
starcount = 512
starcolors = {
	Color(255,0,0),
	Color(255,255,0),
	Color(255,255,255),
	Color(0,255,255),
	Color(0,0,255),
	Color(255,0,255),
	Color(0,255,0),
	Color(0,0,255)
}

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

	for i=1,starcount do
		local x = math.random() - 0.5
		local y = math.random() - 0.5
		local z = math.random() - 0.5
		local m = math.sqrt(x*x + y*y + z*z)
		x = x / m; y = y / m; z = z / m

		table.insert(starfield, {
			color = starcolors[math.random(1,#starcolors)],
			pos = Vector3(x,y,z) * (math.random(0.0,1.0) * starradius)
        })
	end

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

	if GetKey(KEY_SPACE) then
		camera.pos = camera.pos + Vector3(0,1,0) * dt * SPEED
	end

	updateCamera(dt)
end

function _render()
	ClearScene(22, 22, 22)
	CameraPerspective(70)
	
	lookAt:bind(VIEW)
	
	for i=1,#starfield
	do
		local w = (rot * Matrix():translate(starfield[i].pos))
		cube:draw(w)
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
