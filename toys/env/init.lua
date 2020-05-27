local sphere
local sphereModel
local time = 0
local light
local testFont
local rt
local mat
local rtMat

function _init()
    sphereModel = Model("sphere.fbx")
    sphere = sphereModel:getRootNode()
    mat = sphereModel:getMeshes()[1]:getMaterial(1)

    light = Light()
    light:setDirection(Vector(-1,-1,1))
    light:setType(LIGHTKIND_DIRECTIONAL)
    light:enable(true, 0)

    RegisterFontFile("slkscr.ttf")
    testFont = Font("Silkscreen", 36, 1, false)

    local res = GetResolution()
    rt = RenderTarget(math.floor(res[1]/4), math.floor(res[2]/4), true)
    rtMat = Material()
    rtMat:setHandle(1, rt:getHandle())
end

function _update(dt)
    if GetKeyDown(KEY_ESCAPE) then
        ExitGame()
    end

    time = time + dt
    sphere:setTransform(Matrix():rotate(math.sin(time), 0, 0))
end

function _render()
    rt:bind()
    Matrix():lookAt(
        Vector(0,0,-5),
        Vector(),
        Vector(0,1,0)
    ):bind(VIEW)
    CameraPerspective(62, 0.1, 100)

    EnableLighting(true)

    ClearScene(120,20,69)
    AmbientColor(16,16,16)
    sphereModel:getMeshes()[1]:setMaterial(0, mat)
    sphere:draw(Matrix():scale(2,2,2))

    ClearTarget()
    EnableLighting(false)
    ClearScene(20,20,69)
    AmbientColor(255,255,255)
    sphereModel:getMeshes()[1]:setMaterial(0, rtMat)
    sphere:draw(Matrix())
end

function _render2d()
    testFont:drawText(0xFFFFFFFF, "RENDERCEPTION!!!", 15, 30)
end
