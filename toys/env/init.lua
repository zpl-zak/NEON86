local test
local testModel
local monkey
local sphere
local skybox
local time = 0
local light
local testFont
local rt
local mat
local rtMat

dofile("camera.lua")

function _init()
    testModel = Model("test.fbx")
    test = testModel:getRootNode()
    sphere = Model("sphere.fbx")
    monkey = Model("monkey.fbx")
    mat = testModel:getMeshes()[1]:getMaterial(1)

    skybox = Model("skybox.fbx")
    skybox:getMeshes()[1]:getMaterial(1):setShaded(false)

    light = Light()
    light:setDirection(Vector(0,-1,1))
    light:setType(LIGHTKIND_DIRECTIONAL)
    light:enable(true, 0)

    RegisterFontFile("slkscr.ttf")
    testFont = Font("Silkscreen", 36, 1, false)

    local res = GetResolution()
    rt = RenderTarget(math.floor(res[1]/2), math.floor(res[2]/4), true)
    rtMat = Material()
    rtMat:setHandle(1, rt:getHandle())

    ShowCursor(false)
	SetCursorMode(CURSORMODE_CENTERED)
end

function _update(dt)
    if GetKeyDown(KEY_ESCAPE) then
        ExitGame()
    end

    if GetKeyDown(KEY_F2) then
		ShowCursor(not IsCursorVisible())
		SetCursorMode(1-GetCursorMode())
    end

    time = time + dt
    updateCamera(dt)
end

function _render()
    rt:bind()
    cam = Matrix():lookAt(
        Vector(),
        Vector(0,0,-1),
        Vector(0,1,0)
    ):rotate(math.sin(time), 0, 0)
    camMat = lookAt
    cam:bind(VIEW)
    CameraPerspective(90, 0.1, 1000)

    EnableLighting(false)

    ClearScene(120,20,69)
    AmbientColor(16,16,16)

    sphere:draw(Matrix():translate(2+math.cos(time)*1.2,0,-4))
    skybox:draw(Matrix())
    monkey:draw(Matrix():translate(camera.pos))

    ClearTarget()
    camMat:bind(VIEW)
    CameraPerspective(62, 0.1, 1000)
    EnableLighting(false)
    ClearScene(20,20,69)
    testModel:getMeshes()[1]:setMaterial(0, rtMat)
    test:draw(Matrix():rotate(math.sin(time), 0, 0):scale(2,1,1))
    sphere:draw(Matrix():translate(2 +math.cos(time)*1.2,0,-4))
    skybox:draw(Matrix())
    --monkey:draw(Matrix():translate(camera.pos):translate(0,-1,0))
end

function _render2d()
    testFont:drawText(0xFFFFFFFF, "RTT mirrors test (no shaders)", 15, 30)
end
