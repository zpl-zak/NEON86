local terrain
local terrainEffect
local fxaaEffect
local light
local screenRT
local invTexSize

time = 0.0

useShaders = true
useFXAA = true
alphaValue = 1.0

campos = Vector3(-7, 7, -7, 0)

lookAt = Matrix():lookAt(
	campos,
	Vector3(0),
	Vector3(0,1,0)
)

function _init()
	terrain = Model("mountains.obj", true, true)
	terrainEffect = Effect("terrain.fx")
	fxaaEffect = Effect("fxaa.fx")

	local res = GetResolution()
	screenRT = RenderTarget(res[1], res[2])
	local aspect = res[1] / res[2]
	local hAdd = aspect * 150.0
	invTexSize = Vector3(
		1.0 / res[1],
		1.0 / (res[2] + hAdd),
		0.0
	)

	light = Light(0)
	light:setType(LIGHTKIND_DIRECTIONAL)
	light:setDirection(Vector3(4.0, 3.0, -5.0)*-1)
	light:setDiffuse(Vector3(0.91, 0.58, 0.13):color())
end

function _fixedUpdate(dt)
	if GetKeyDown(KEY_ESCAPE) then
		ExitGame()
	end

	if GetKeyDown(KEY_F3) then
		useShaders = not useShaders
	end

	if GetKeyDown("f") then
		useFXAA = not useFXAA
	end

	time = time + dt

	campos = campos * Matrix():rotate(math.rad(math.sin(time/4)*0.25), 0, 0)

	lookAt = Matrix():lookAt(
		campos,
		Vector3(0),
		Vector3(0,1,0)
	)
end

function _render()
	screenRT:bind()
	ClearScene(20,20,20)
	CameraPerspective(70)
	AmbientColor(20,20,69)
	lookAt:bind(VIEW)

	light:enable(true, 0)
	EnableLighting(not useShaders)

	if useShaders then
		terrainEffect:start("TerrainRender")
		terrainEffect:beginPass(1)
		terrainEffect:setVector3("campos", campos)
		terrainEffect:setFloat("alphaValue", 1)
		terrainEffect:setFloat("time", time)
		terrainEffect:setLight("light", light)
		terrainEffect:commit()
	end

	terrain:draw(Matrix():scale(10,10,10))

	if useShaders then
		terrainEffect:endPass()
		terrainEffect:finish()
	end

	ClearTarget()

	if useFXAA then
		fxaaEffect:start("FXAA")
		fxaaEffect:beginPass(1)
		fxaaEffect:setTexture("srcTex", screenRT)
		fxaaEffect:setVector3("inverseTexSize", invTexSize)
		fxaaEffect:setFloat("fxaaReduceMul", 1.0 / 8.0)
		fxaaEffect:setFloat("fxaaReduceMin", 1.0 / 128.0)
		fxaaEffect:setFloat("fxaaSpanMax", 8.0)
		fxaaEffect:commit()

		FillScreen()

		fxaaEffect:endPass()
		fxaaEffect:finish()
	else
		BindTexture(0, screenRT)
		FillScreen()
		BindTexture(0)
	end
end
