local Class = require "class"
local Camera = require "camera"
local Collisions = require "collisions"

Class "GameCamera" (Camera) {
    -- Override movement method for collisions

    __init__ = function (self, pos, cols)
        Camera:__init__(pos)
        self.grounded = false
        self.cols = cols
        self.speed = 1
    end,

    movement = function (self, dt)
        self.movedir:y(0)
        if self.grounded == false then
            self.vel:y(self.vel:y() - 2*dt)
        end
        self.vel:x(self.vel:x() + self.movedir:x())
        self.vel:z(self.vel:z() + self.movedir:z())

        self.cols:forEach(function (shape)
            shape:testSphere(
                self.pos,
                3,
                self.vel:normalize(),
                function (norm, pd, tr)
                    local push = (norm:normalize()*pd)
                    self.pos = self.pos + push*0.0001

                    local pp = push * ((self.vel * push) / (push * push))
                    self.vel = (self.vel - pp)
                    self.grounded = true
                end
            )
        end)

        self.pos = self.pos + self.vel
    end,

    update2 = function (self, dt)
        self.grounded = false
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
        self.cam = GameCamera(
            self.nodes.cament:row(4),
            self.cols)

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
        local w = Collisions.newWorld()
        
        -- Terrain
        local n = self.nodes.terrain
        local mp = n:getMeshParts()[1][1]
        w:addCollision(
            Collisions.newTriangleMeshFromPart(
                mp,
                n:getFinalTransform()
            )
        )

        -- Props
        n = self.nodes.props
        for _, pn in pairs(n:getNodes()) do
            local p = pn:getMeshes()[1]
            for _, pp in pairs(p:getParts()) do
                w:addCollision(
                    Collisions.newTriangleMeshFromPart(
                        pp,
                        pn:getFinalTransform()
                    )
                )
            end
        end

        return w
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