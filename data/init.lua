model = Model("cube.obj")

dofile("camera.lua")

time = 0.0

testEffect = Effect("test.fx")

renderMode = "PointLighting"
alphaValue = 1.0

function _init()
	ShowCursor(false)
	SetCursorMode(CURSORMODE_CENTERED)
end

function _update(dt)
	if GetKeyDown(KEY_F2) then
		ShowCursor(not IsCursorVisible())
		SetCursorMode(1-GetCursorMode())
	end 

	if GetKeyDown(KEY_ESCAPE) then
		ExitGame()	
	end

	if GetKeyDown(KEY_F3) then
		if renderMode == "PointLighting" then
			renderMode = "AmbientLighting"
		else
			renderMode = "PointLighting"
		end
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
	CameraPerspective(62)
	
	lookAt:bind(VIEW)

	testEffect:start(renderMode)

	testEffect:beginPass(1)

	testEffect:setVector3("campos", camera.pos)
	testEffect:setVector4("globalAmbient", Vector4(0.12,0.12,0.12))
	testEffect:setFloat("alphaValue", alphaValue)
	testEffect:setFloat("time", time)
	testEffect:commit()

	model:draw(Matrix():scale(10,10,10))

	testEffect:endPass()

	testEffect:finish()
end
