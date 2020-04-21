tristram = Model("tristram.glb")

dofile("camera.lua")

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
end

function _render()
	ClearScene(20,20,20)
	CameraPerspective(70)
	
	lookAt:bind(VIEW)

	local numPasses = testEffect:start(renderMode)

	for i=1,numPasses
	do
		testEffect:beginPass(i)
		testEffect:setVector3("campos", camera.pos)
		testEffect:setVector4("globalAmbient", Vector3(0.12), 1.0)
		testEffect:setVector4("lights[0].diffuse", Vector3(1.0), 1.0)
		testEffect:setFloat("alphaValue", alphaValue)
		testEffect:commit()

		tristram:draw(Matrix())
		testEffect:endPass()
	end

	testEffect:finish()
end
