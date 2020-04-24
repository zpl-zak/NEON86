scale = 1

dofile("camera.lua")
dofile("solar.lua")

time = 0.0

spaceFX = Effect("space.fx")

alphaValue = 1.0


demoSystem = generateSystem(Vector(), 24, 4, 30)

skybox = Model("skybox.fbx")

function _init()
	ShowCursor(false)
	SetCursorMode(CURSORMODE_CENTERED)

	math.randomseed(1337)
end

function _update(dt)
	if GetKeyDown(KEY_F2) then
		ShowCursor(not IsCursorVisible())
		SetCursorMode(1-GetCursorMode())
	end 

	if GetKeyDown(KEY_ESCAPE) then
		ExitGame()	
	end

	if GetKeyDown(KEY_F4) then
		alphaValue = alphaValue + 0.5

		if alphaValue > 1.0 then
			alphaValue = 0.0
		end
	end

	if GetKeyDown(KEY_F5) then
		alphaValue = 1.0
	end

	updateCamera(dt)

	time = time + dt
end

function _render()
	ClearScene(20,20,20)
	CameraPerspective(62, 0.1, 10000)
	
	lookAt:bind(VIEW)

	spaceFX:start("Main")

	spaceFX:beginPass(1)

	spaceFX:setVector3("campos", camera.pos)
	spaceFX:setVector4("globalAmbient", Vector4(0.23,0.23,0.23))
	spaceFX:setFloat("alphaValue", alphaValue)
	spaceFX:setFloat("time", time)

	drawSystem(demoSystem)
	
	spaceFX:endPass()
	spaceFX:finish()

	--[[ unlit ]]
	skybox:draw(Matrix():scale(scale*20):translate(camera.pos))
	
end
