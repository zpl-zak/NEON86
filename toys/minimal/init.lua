local humanFactory = require "human"
local humanModel
local scene
local floor
local time = 0
local viewMat

local lights = {}
local enemies = {}

function _init()
    humanModel = Model("human.fbx")
    scene = Model("scene.fbx")
    floor = scene:findMesh("Plane")
    
    local mat = Material("textures/cube_albedo.png")
    mat:setOpacity(0.5)
    floor:setMaterial(0, mat)

    for name, tgt in pairs(scene:getTargets()) do
        if string.find(name, "enemy_spawn") ~= nil then
            addEnemy(tgt, humanModel)
        end
    end

    addLight(0, LIGHTKIND_DIRECTIONAL, Vector(), Vector3(-1.0, -0.3, -1.0), Vector3(4,1,1,1))
    
    EnableLighting(true)
end

function _update(dt)
    if GetKeyDown(KEY_ESCAPE) then
        ExitGame()
    end

    if GetKeyDown("m") then
        shaderDisabled = not shaderDisabled
    end

    for i, enemy in ipairs(enemies) do
        enemy.model:update(dt)
    end

    time = getTime()
end

function _render()
    viewMat = Matrix()
        :rotate(time/4, 0, 0)
        :rotate(0, math.rad(-25), 0)
        :translate(0,3,15)
        :bind(VIEW)

    for i, l in ipairs(lights) do
        l:setSlot(i)
        l:enable(true)
    end

    ClearScene(20,20,69)
    AmbientColor(128,128,128)
    CameraPerspective(62, 2, 40)
    
    floor:draw(Matrix():translate(0,-2,0))

    for _, m in ipairs(enemies) do
        m.model:draw(Matrix():scale(2,2,2):translate(m.pos):translate(0,-2,0))
    end
end

function addEnemy(mat, model)
    table.insert(enemies, {
        model = humanFactory(model),
        pos = mat:row(4)
    })
end

function addLight(slot, kind, pos, dir, diffuse)
    local l = Light(slot)
    l:setType(kind)
    l:setPosition(pos)
    l:setDirection(dir)
    l:setDiffuse(diffuse:color())

    table.insert(lights, l)
end
