local sphere
local sphereModel
local time = 0
local light

function _init()
    sphereModel = Model("sphere.fbx")
    sphere = sphereModel:getRootNode()

    light = Light()
    light:setDirection(Vector(-1,-1,1))
    light:setType(LIGHTKIND_DIRECTIONAL)
    light:enable(true, 0)
    EnableLighting(true)
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

    sphere:draw(Matrix())
end
