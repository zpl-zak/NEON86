local scene
local world
local time = 0
local globe
local rootNode

local lights = {}
local enemies = {}

function _init()
    scene = Scene("scene.fbx")
    rootNode = scene:getRootNode()
    world = rootNode:findNode("World")
    globe = rootNode:findNode("Planet")

    globe:getMeshes()[1]:getMaterial(1):alphaIsTransparency(false)

    for _, n in pairs(scene:getFlattenNodes()) do
        local shaded = n:getMeta("shaded")

        if shaded ~= nil then
            for _, m in pairs(n:getMeshes()[1]:getParts()) do
                m:getMaterial():setShaded(shaded == "true")
            end
        end
    end

    addLight(LIGHTKIND_DIRECTIONAL, Vector(), Vector3(-1.0, -0.8, -1.0), Vector3(0.6,0.6,0.2,1))

    local lightsNode = rootNode:findNode("Lights")

    for _, l in pairs(lightsNode:getNodes()) do
        local c = VectorRGBA(240,240,240)

        local p = l:getMeta("color")

        if p ~= nil then
            c = str2vec(p) / 0xFF
        end

        addLight(LIGHTKIND_POINT, l:getFinalTransform():row(4), Vector(), c)
    end

    EnableLighting(true)
end

function _update(dt)
    if GetKeyDown(KEY_ESCAPE) then
        ExitGame()
    end

    if GetKeyDown("r") then
        RestartGame()
    end

    for i, enemy in ipairs(enemies) do
        enemy.model:update(dt)
    end

    time = getTime()
end

function _render()
    Matrix()
        :rotate(time/4, 0, 0)
        :rotate(0, math.rad(-35), 0)
        :translate(0,-2,20)
        :bind(VIEW)

    for i, l in ipairs(lights) do
        l:enable(true, i-1)
    end

    ClearScene(20,20,69)
    AmbientColor(16,16,16)
    CameraPerspective(62, 2, 80)

    world:draw(Matrix())
    scene:getRootNode():findNode("Enemies posing"):draw(Matrix())

    globe:draw(Matrix():rotate(-time/4 + math.rad(45), 0, 0))
end

function addLight(kind, pos, dir, diffuse)
    local l = Light()
    l:setType(kind)
    l:setPosition(pos)
    l:setDirection(dir)
    l:setSpecular(0.12,0.12,0.12,1)
    l:setDiffuse(diffuse)

    if kind == LIGHTKIND_POINT then
        l:setRange(50)
        l:setAttenuation(0,0.2,0)
    end

    table.insert(lights, l)
end
