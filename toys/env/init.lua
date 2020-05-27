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

local changeRot = 0

dofile("camera.lua")

function _init()
    testModel = Model("test.fbx")
    testModel:getRootNode():setTransform(Matrix():scale(2,1,1))
    sphere = Model("sphere.fbx")
    monkey = Model("monkey.fbx")

    skybox = Model("skybox.fbx")
    skybox:getMeshes()[1]:getMaterial(1):setShaded(false)

    RegisterFontFile("slkscr.ttf")
    testFont = Font("Silkscreen", 18, 1, false)

    local res = GetResolution()
    rt = RenderTarget(2048, 2048, true)
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

    if GetKeyDown("m") then
        changeRot = 1 - changeRot
    end

    time = time + dt
    updateCamera(dt)
end

function _render()
    rt:bind()
    CameraPerspective(62.5, 0.1, 500, true)

    cam = Matrix():lookAt(
        Vector(),
        lookAt:row(3),
        Vector(0,1,0)
    )

    CullMode(2)

    cam:bind(VIEW)
    EnableLighting(false)
    ClearScene(120,20,69)
    AmbientColor(16,16,16)

    skybox:draw(Matrix():translate(camera.pos))
    sphere:draw(Matrix():translate(2+math.cos(time)*1.2,0,-4))
    monkey:draw(Matrix():translate(camera.pos))
    testModel:getMeshes()[1]:setMaterial(0, rtMat)
    testModel:draw()

    rt:bind()
    CullMode(3)
    lookAt:bind(VIEW)
    CameraPerspective(62, 0.1, 500)
    EnableLighting(false)
    ClearScene(20,20,69)

    skybox:draw(Matrix():translate(camera.pos))
    sphere:draw(Matrix():translate(2 +math.cos(time)*1.2,0,-4))
    testModel:draw()

    ClearTarget()
    ClearScene(120,20,69)
    skybox:draw(Matrix():translate(camera.pos))
    sphere:draw(Matrix():translate(2 +math.cos(time)*1.2,0,-4))
    testModel:draw()
end

function _render2d()
    testFont:drawText(0xFFFFFFFF, "RTT mirrors test (no shaders)\nWASD to move around\nM to toggle mirror rotation", 15, 30)
end
