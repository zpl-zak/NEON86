scale = 1
local camera
local cam = require "camera"

dofile("solar.lua")

paused = false
time = 0.0

spaceFX = Effect("space.fx")

alphaValue = 1.0


demoSystem = generateSystem(Vector(), 24, 4, 30)

skybox = Model("skybox.fbx")

fontSize = 20
font = Font("Arial", fontSize, 700)

function _init()
	ShowCursor(false)
	SetCursorMode(CURSORMODE_CENTERED)

	math.randomseed(1337)
	camera = cam.newCamera(Vector3(0,6,-540*scale))
	camera.speed = 120.0*scale
end

function _update(dt)
	if GetKeyDown(KEY_F2) then
		ShowCursor(not IsCursorVisible())
		SetCursorMode(1-GetCursorMode())
	end

	if GetKeyDown(KEY_ESCAPE) then
		ExitGame()
	end

	if GetKeyDown("p") then
		paused = not paused
	end

	camera:update(dt)

	if paused == false then
		time = time + dt
	end
end

function _render()
	ClearScene(20,20,20)
	CameraPerspective(62, 0.1, 10000)

	camera.mat:bind(VIEW)

	spaceFX:start("Main")

	spaceFX:beginPass("main")

	spaceFX:setVector3("campos", camera.pos)
	spaceFX:setVector4("globalAmbient", Vector4(0.01,0.01,0.01))
	spaceFX:setFloat("alphaValue", alphaValue)
	spaceFX:setFloat("time", time)

	drawSystem(demoSystem)

	spaceFX:endPass()
	spaceFX:finish()

	--[[ unlit ]]
	skybox:draw(Matrix():scale(scale*20):translate(camera.pos))

end

function _render2d()
	font:drawText(0xffffffff, "WASD to move\nPress 'P' to pause simulation\nPress 'F2' to unfocus cursor", 15, 30 + 0)
end
