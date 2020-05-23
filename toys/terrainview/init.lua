local terrain
local terrainEffect
local light

time = 0.0

useShaders = true
alphaValue = 1.0

campos = Vector3(-7, 7, -7, 0)
ambience = VectorRGBA(20,20,69)

lookAt = Matrix():lookAt(
	campos,
	Vector3(0),
	Vector3(0,1,0)
)

function _init()
	terrain = Model("mountains.obj", true, true)
	terrainEffect = Effect("terrain.fx")
	light = Light(0)
	light:setType(LIGHTKIND_DIRECTIONAL)
	light:setDirection(Vector3(4.0, 3.0, -5.0)*-1)
	light:setDiffuse(Vector3(0.91, 0.58, 0.13):color())
end

function _update(dt)
	if GetKeyDown(KEY_ESCAPE) then
		ExitGame()
	end

	if GetKeyDown(KEY_F3) then
		useShaders = not useShaders
	end

	time = time + dt

	campos = campos * Matrix():rotate(dt/4, 0, 0)

	lookAt = Matrix():lookAt(
		campos,
		Vector3(0),
		Vector3(0,1,0)
	)
end

function _render()
	ClearScene(20,20,20)
	CameraPerspective(70)
	AmbientColor((ambience*0xFF):color())
	lookAt:bind(VIEW)

	light:enable(true, 0)
	EnableLighting(not useShaders)

	if useShaders then
		terrainEffect:start("PointLighting")
		terrainEffect:beginPass("main")
		terrainEffect:setVector3("campos", campos)
		terrainEffect:setVector4("globalAmbient", ambience)
		terrainEffect:setFloat("alphaValue", 1)
		terrainEffect:setFloat("time", time)
		terrainEffect:commit()
	end

	terrain:draw(Matrix():scale(10,10,10))

	if useShaders then
		terrainEffect:endPass()
		terrainEffect:finish()
	end
end
