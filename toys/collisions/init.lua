local sphere
local sphereModel
local time = 0
local light
local testFont
local roomScene
local room
local world
local cols = require "collisions"
local gravity = -0.0981

local DEFAULT_SPHERE_POS = Vector(0,6,0)
local DEFAULT_BOUNCE_FACTOR = 0.90
local DEFAULT_SPHERE_VEL_CAP = 0.001
local bounceFactor = DEFAULT_BOUNCE_FACTOR

local roomSpawnBox
roomSides = {}
local balls = {}

dofile("camera.lua")

function _init()
    sphereModel = Model("sphere.fbx", false)
    sphere = sphereModel:getRootNode()

    roomScene = Scene("scene.fbx")
    roomRoot = roomScene:getRootNode()
    room = roomRoot:findNode("room")
    roomSpawnBox = roomRoot:findNode("roomBounds"):getMeshParts()[1][1]:getBounds()
    addBall()

    local cament = roomScene:findTarget("campos")
    local campos = cament:row(4)
    local camdir = cament:row(2)
    local camup = cament:row(3):neg()

    Matrix():lookAt(
        campos,
        campos+camdir,
        camup
    ):bind(VIEW)

    for i, side in pairs(room:getNodes()) do
        local dims = side:getMeshParts()[1][1]:getBounds()
        local mat = side:getTransform()
        local tdims = cols.newBox(dims):withMatrix(mat)
        tdims = tdims:withDelta(Vector(0,10,10))
        LogString("Room side " .. tostring(i) .. ": min(" .. vec2str(tdims.min) .. "), max(" .. vec2str(tdims.max) .. ")")
        table.insert(roomSides, tdims)
    end

    light = Light()
    light:setDirection(Vector(-1,-1,1))
    light:setType(LIGHTKIND_DIRECTIONAL)
    light:enable(true, 0)
    EnableLighting(true)

    RegisterFontFile("slkscr.ttf")
    testFont = Font("Silkscreen", 18, 1, false)
end

function _update(dt)
    updateCamera(dt)
    lookAt:bind(VIEW)

    if GetKeyDown(KEY_ESCAPE) then
        ExitGame()
    end

    if GetMouse(MOUSE_RIGHT_BUTTON) then
        ShowCursor(false)
    	SetCursorMode(CURSORMODE_CENTERED)
    else
        ShowCursor(true)
    	SetCursorMode(CURSORMODE_DEFAULT)
    end

    if GetKeyDown(KEY_SPACE) then
        addBall()
    end

    if GetKeyDown("m") then
        bounceFactor = 1.35
    end

    if GetKeyDown("b") then
        bounceFactor = 0.5
    end

    if GetKeyDown("n") then
        bounceFactor = DEFAULT_BOUNCE_FACTOR
    end

    if GetKeyDown("t") then
        balls = {}
        addBall()
    end

    time = time + dt
    updateBalls(dt)
end

function _render()
    ClearScene(20,20,69)
    AmbientColor(45,45,45)
    CameraPerspective(62, 0.1, 100)

    room:draw()

    ToggleWireframe(true)
    for _, side in pairs(roomSides) do
       DrawBox(Matrix():translate(side.mat:row(4)), side.dims, 0xFFFFFFFF)
    end
    ToggleWireframe(false)

    for _, ball in pairs(balls) do
        sphere:draw(Matrix():translate(ball.pos))
    end
end

function _render2d()
    testFont:drawText(0xFFFFFFFF, [[
Simple sphere physics
Press SPACE to spawn another ball!
Hold RMB to look around
M - bounce: crazy
N - bounce: normal
B - bounce: less bouncy
T - reset scene
    ]], 15, 30)
end

function addBall()
    local opos = Vector3(
        math.random(roomSpawnBox[1]:x(), roomSpawnBox[2]:x()),
        math.random(roomSpawnBox[1]:y(), roomSpawnBox[2]:y()),
        math.random(roomSpawnBox[1]:z(), roomSpawnBox[2]:z())
    )

    local ovel = Vector3(
        math.random(-DEFAULT_SPHERE_VEL_CAP,DEFAULT_SPHERE_VEL_CAP),
        math.random(-DEFAULT_SPHERE_VEL_CAP,DEFAULT_SPHERE_VEL_CAP),
        math.random(-DEFAULT_SPHERE_VEL_CAP,DEFAULT_SPHERE_VEL_CAP)
    )

    LogString("Room min: " .. vec2str(roomSpawnBox[1]) .. ", Room max: " .. vec2str(roomSpawnBox[2]))
    LogString("New ball at: " .. vec2str(opos) .. " with velocity: " .. vec2str(ovel))

    table.insert(balls, {
--         pos = opos,
        vel = ovel,
        pos = Vector3(0,8,0),
--         vel = Vector3(0.04,0,0)
    })
end

function updateBalls(dt)
    for _, ball in pairs(balls) do
        local vel = ball.vel:get()

        for _, side in pairs(roomSides) do
            for i=1,3 do
                local v = Vector():m(i, vel[i])
                ok, delta = side:intersectsSphere(ball.pos, 2, v)

                if ok then
                    ball.vel = ball.vel:m(i, vel[i]*-1*bounceFactor)
                    ball.pos += delta
                    i=99
                end
            end
        end

--         for _, sndBall in pairs(balls) do
--             local dist = (sndBall.pos - ball.pos):mag()

--             if dist <= 4 then
--                 ball.vel = ball.vel:neg()
--                 sndBall.vel = sndBall.vel:neg()
--             end
--         end

        ball.pos += ball.vel
        ball.vel:y(ball.vel:y()+gravity*dt)
    end
end
