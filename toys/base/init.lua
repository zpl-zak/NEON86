local sphere
local sphereModel
local time = 0
local light
local testFont

function _init()
    sphereModel = Model("sphere.fbx")
    sphere = sphereModel:getRootNode()

    light = Light()
    light:setDirection(Vector(-1,-1,1))
    light:setType(LIGHTKIND_DIRECTIONAL)
    light:enable(true, 0)
    EnableLighting(true)

    RegisterFontFile("slkscr.ttf")
    testFont = Font("Silkscreen", 36, 1, false)
end

function _update(dt)
    if GetKeyDown(KEY_ESCAPE) then
        ExitGame()
    end

    time = time + dt
    sphere:setTransform(Matrix():rotate(math.sin(time), 0, 0))
end

function _render()
    ClearScene(20,20,69)
    AmbientColor(16,16,16)

    Matrix():lookAt(
        Vector(0,0,-5),
        Vector(),
        Vector(0,1,0)
    ):bind(VIEW)
    CameraPerspective(62, 0.1, 100)

    sphere:draw()
end

function _render2d()
    testFont:drawText(0xFFFFFFFF, "Welcome to the NEON86 basic template!", 15, 30)
end
