local Class = require "class"
local Camera = require "camera"
local world = require "bullet"

Class "GameCamera" (Camera) {
    -- Override movement method for collisions
    __init__ = function (self, pos, angles)
        Camera:__init__(pos, angles)
        self.grounded = false
        self.speed = 50
        self.body = world.createCapsule(Matrix():translate(pos), 1, 2, 2)
        world.setActivationState(self.body, world.DISABLE_DEACTIVATION)
        world.setDamping(self.body, 0.9, 1)
        world.setFriction(self.body, 0.6)
        world.setAngularFactor(self.body, Vector(0,1,0))
        -- world.setGravity(self.body, Vector())
    end,

    movement = function (self, dt)
        self.movedir:y(0)
        world.addImpulse(self.body, self.movedir)
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
    end
}

Class "Game" {
    __init__ = function (self, name)
        self.scene = Scene(name)
        self.root = self.scene:getRootNode()
        self.nodes = {
            terrain = self.root:findNode("terrain"),
            props = self.root:findNode("props"),
            cament = self.root:findTarget("cament")
        }
        self.cols = self:buildCols()
        self.cam = GameCamera(self.nodes.cament:row(4))

        self.sun = self:setupLight()
    end,

    -- Events

    update = function (self, dt)
        self.cam:update(dt)
        self.cam:update2(dt)
    end,

    render = function (self)
        self.cam.mat:bind(VIEW)

        self.sun:enable(true, 0)
        self.nodes.terrain:draw()
        self.nodes.props:draw()
    end,

    -- Helpers

    buildCols = function (self)
        -- Terrain
        local n = self.nodes.terrain
        local mp = n:getMeshes()[1]
        local ground = world.createMesh(n:getFinalTransform(), mp)
        world.setRestitution(ground, 0.9)

        -- Props
        n = self.nodes.props
        for _, pn in pairs(n:getNodes()) do
            local p = pn:getMeshes()[1]
            local prop = world.createMesh(pn:getFinalTransform(), p)
            world.setRestitution(prop, 0.7)
        end
    end,

    setupLight = function (self)
        local light = Light()
        light:setDirection(Vector(-1,-0.2,1))
        light:setType(LIGHTKIND_DIRECTIONAL)
        light:setDiffuse(255,128,0)
        light:enable(true, 0)
        return light
    end,
}

local g = Game("desert.fbx")

RegisterFontFile("slkscr.ttf")
local testFont = Font("Silkscreen", 18, 1, false)

ShowCursor(false)
SetCursorMode(CURSORMODE_CENTERED)

SetFog(VectorRGBA(80,80,69), FOGKIND_EXP, 0.008)
EnableLighting(true)

function _update(dt)
    if GetKeyDown(KEY_ESCAPE) then
        ExitGame()
    end

    g:update(dt)
    world:update(dt)
end

function _render()
    ClearScene(80,80,69)
    AmbientColor(80,80,132)
    CameraPerspective(62, 0.1, 1000)
    g:render()
end

function _render2d()
    testFont:drawText(0xFFFFFFFF, [[
FPS camera with collisions
WASD to move around
    ]], 15, 30)
end
