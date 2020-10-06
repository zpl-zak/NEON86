local world = require "bullet"
local Camera = require "camera"
local Class = require "class"
local helpers = require "helpers"

Class "GameCamera" (Camera) {
    -- Override movement method for collisions
    __init__ = function (self, pos, angles)
        Camera:__init__(pos, angles)
        self.grounded = false
        self.speed = 500
        self.body = world.createCapsule(Matrix():translate(pos), 1, 2, 2)
        world.setActivationState(self.body, world.DISABLE_DEACTIVATION)
        -- world.setDamping(self.body, 0.9, 1)
        world.setFriction(self.body, 0.9)
        world.setAngularFactor(self.body, Vector(0,1,0))
    end,

    movement = function (self, dt)
        local hvel = world.getVelocity(self.body)
        self.vel = helpers.lerp(self.vel, self.movedir, 0.4)
        self.vel:y(hvel:y())
        -- world.addImpulse(self.body, self.movedir)
        world.setVelocity(self.body, self.vel)
    end,

    updateMatrix = function (self)
        self.pos = world.getWorldTransform(self.body):row(4)
        self.mat = Matrix():translate(self.pos:neg()-Vector3(0,1,0))
                            :rotate(-self.angles[1], 0, 0)
                            :rotate(0, self.angles[2], 0)
        self:updateDirVectors()
    end,

    update2 = function (self, dt)
        self.grounded = false
    end,

    setPos = function (self, pos)
        self.pos = pos
        world.setWorldTransform(self.body, Matrix():translate(pos))
        world.setVelocity(self.body, Vector())
    end
}

local cam = GameCamera(Vector3(0,3,-10))
local balls = {}
local ballSize = 1.0
local bowl = Model("bowl.fbx")
local node = bowl:getRootNode():findNode("Cube")
local plane = world.createMesh(node:getFinalTransform(), node:getMeshes()[1])
world.setRestitution(plane, 0.9)

for _, mesh in pairs(node:getMeshes()) do
    for _, part in pairs(mesh:getParts()) do
        part:getMaterial(1):setSamplerState(SAMPLERSTATE_MAGFILTER, TEXF_POINT)
        part:getMaterial(1):setSamplerState(SAMPLERSTATE_MINFILTER, TEXF_POINT)
        part:getMaterial(1):setSamplerState(SAMPLERSTATE_MIPFILTER, TEXF_POINT)
    end
end

function addBall(pos)
    if #balls >= 50 then
        world.destroy(balls[1])
        table.remove(balls, 1)
    end
    local ball = world.createSphere(pos, ballSize, 1)
    world.setRestitution(ball, 0.7)
    table.insert(balls, ball)
    return ball
end

addBall(Matrix():translate(Vector(math.random()*0.1,5,math.random()*0.1)))

sphere = Model("sphere.fbx")
bgColor = Color(60, 60,69)
ambColor = Color(40,40,69)

light = Light()
light:setDirection(Vector(-0.5,-0.5,1))
light:enable(true, 0)

function _update(dt)
    if GetKeyDown(KEY_ESCAPE) then
        ExitGame()
    end
    
    ShowCursor(false)
    SetCursorMode(CURSORMODE_CENTERED)
    
    if GetKey(KEY_SPACE) then
        addBall(Matrix():translate(Vector(math.random()*0.1,5,math.random()*0.1)))
    end
    
    if GetMouseDown(MOUSE_RIGHT_BUTTON) then
        local handle, dpos = world.rayTest(cam.pos, cam.dirs.fwd * 100)
        
        if handle ~= nil then
            cam:setPos(dpos+Vector(0,1,0))
        end
    end
    
    if GetMouseDown(MOUSE_LEFT_BUTTON) then
        local handle = addBall(Matrix():translate(cam.pos + cam.dirs.fwd*2))
        world.addImpulse(handle, cam.dirs.fwd*20)
    end
    
    world.update()
    cam:update(dt)
    cam:update2(dt)
end

function _render()
    ClearScene(bgColor)
    AmbientColor(ambColor)
    CameraPerspective(62, 0.1, 100)
    EnableLighting(true)
    cam.mat:bind(VIEW)
    
    bowl:draw()
    
    for i, handle in pairs(balls) do
        sphere:draw(
            Matrix():scale(ballSize, ballSize, ballSize) * world.getWorldTransform(handle)
        )
    end
end

local testFont = Font("Arial", 33, 2000, false)

function _render2d()
    testFont:drawText(0xFFFFFFFF, [[
        WASD to move around
        LMB to shoot balls
        RMB to teleport at aimed location
        SPACE for waterfall
        ]], 15, 30)
    end
    