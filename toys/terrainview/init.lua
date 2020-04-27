terrain = Model("mountains.obj")

time = 0.0

terrainEffect = Effect("terrain.fx")

renderMode = "PointLighting"
alphaValue = 1.0

campos = Vector3(-7, 7, -7)

lookAt = Matrix():lookAt(
	campos,
	Vector3(0),
	Vector3(0,1,0)
)

function _init()
	
end

function _update(dt)
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

	time = time + dt

	-- campos:x(campos:x() + math.cos(time))
	-- campos:z(campos:z() - math.tan(time))
	-- campos:x(campos:x() - math.sin(time))

	campos = campos * Matrix():rotate(0, dt/4, 0)

	lookAt = Matrix():lookAt(
		campos,
		Vector3(0),
		Vector3(0,1,0)
	)
end

function _render()
	ClearScene(20,20,20)
	CameraPerspective(70)
	
	lookAt:bind(VIEW)

	terrainEffect:start(renderMode)

	terrainEffect:beginPass(1)

	terrainEffect:setVector3("campos", campos)
	terrainEffect:setVector4("globalAmbient", Vector3(0.12), 1.0)
	terrainEffect:setFloat("alphaValue", 1)
	terrainEffect:setFloat("time", time)
	terrainEffect:commit()

	terrain:draw(Matrix():scale(10,10,10))

	terrainEffect:endPass()

	terrainEffect:finish()
end
