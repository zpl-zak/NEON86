spinner = 0.0
cube = nil
rot = nil
lookAt = nil
tristram = Model("tristram.glb")

sponza = MeshGroup()
--sponza:loadMesh("sponza.gltf")

dofile("camera.lua")

SPEED = 5.0
SENSITIVITY = 0.15

starfield = {}
starradius = 64
starcount = 0
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
	
	cube = Model()
	cube:loadModel("cube.glb")

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

	local vel = Vector3()

	if GetKey("w") then
		vel = vel + Vector3(camera.fwdl * dt * SPEED)
	end

	if GetKey("s") then
		vel = vel + Vector3(camera.fwdl * dt * SPEED):neg()
	end

	if GetKey("a") then
		vel = vel + Vector3(camera.rhs * dt * SPEED):neg()
	end

	if GetKey("d") then
		vel = vel + Vector3(camera.rhs * dt * SPEED)
	end

	if GetKey(KEY_SHIFT) then
		vel = vel * 2
	end

	if vel:mag() ~= 0 then
		camera.vel = camera.vel + (Vector3(vel) - camera.vel)*0.10
		camera.heldControls = true
	end

	updateCamera(dt)
end

function _render()
	ClearScene(196, 232, 255)
	CameraPerspective(70)
	
	lookAt:bind(VIEW)

	tristram:draw(Matrix())

	sponza:draw(Matrix():translate(0,5, 10))
	
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
