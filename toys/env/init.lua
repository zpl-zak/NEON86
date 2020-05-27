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

local changeRot = 1

dofile("camera.lua")

function _init()
    testModel = Model("test.fbx")
    sphere = Model("sphere.fbx")
    monkey = Model("monkey.fbx")

    skybox = Model("skybox.fbx")
    skybox:getMeshes()[1]:getMaterial(1):setShaded(false)

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

    if GetKeyDown("m") then
        changeRot = 1 - changeRot
    end

    time = time + dt
    updateCamera(dt)
end

function _render()
    rt:bind()
    CameraPerspective(75, 0.1, 1000, true)

    cam = Matrix():lookAt(
        Vector(),
        Vector(0,0,1),
        Vector(0,1,0)
    )

    CullMode(2)

    if changeRot > 0 then
        cam = cam:rotate(math.sin(time), 0, 0)
    end

    cam:bind(VIEW)
    EnableLighting(false)
    ClearScene(120,20,69)
    AmbientColor(16,16,16)

    sphere:draw(Matrix():translate(2+math.cos(time)*1.2,0,-4))
    skybox:draw(Matrix())
    monkey:draw(Matrix():translate(camera.pos))

    ClearTarget()
    CullMode(3)
    lookAt:bind(VIEW)
    CameraPerspective(62, 0.1, 1000)
    EnableLighting(false)
    ClearScene(20,20,69)

    testModel:getMeshes()[1]:setMaterial(0, rtMat)
    testMat = Matrix():rotate(math.sin(time)*changeRot, 0, 0)
    testModel:draw(testMat:scale(2,1,1))
    sphere:draw(Matrix():translate(2 +math.cos(time)*1.2,0,-4))
    skybox:draw(Matrix())
end

function _render2d()
    testFont:drawText(0xFFFFFFFF, "RTT mirrors test (no shaders)\nWASD to move around\nM to disable mirror rotation", 15, 30)
end
