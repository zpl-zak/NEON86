local world = require "bullet"
local Camera = require "camera"
local Class = require "class"

Class "GameCamera" (Camera) {
    -- Override movement method for collisions
    __init__ = function (self, pos, angles)
        Camera:__init__(pos, angles)
        self.grounded = false
        self.speed = 50
        self.body = world.createSphere(pos, 1, 2)
        world.disableSleep(self.body)
        world.setDamping(self.body, 0.9, 1)
        world.setFriction(self.body, 0.6)
        -- world.setGravity(self.body, Vector())
    end,

    movement = function (self, dt)
        self.movedir:y(0)
        world.addImpulse(self.body, self.movedir)
    end,

    updateMatrix = function (self)
        self.pos = world.getWorldTransform(self.body):row(4)
        self.mat = Matrix():translate(self.pos:neg())
                            :rotate(-self.angles[1], 0, 0)
                            :rotate(0, self.angles[2], 0)
        self:updateDirVectors()
    end,

    update2 = function (self, dt)
        self.grounded = false
    end
}

local cam = GameCamera(Vector3(0,3,-10))
local balls = {}

local bowl = Model("bowl.fbx")
local node = bowl:getRootNode():findNode("Cube")
local fg = node:getMeshParts()[1][1]
local plane = world.createMesh(node:getFinalTransform(), fg)
world.setRestitution(plane, 0.9)

function addBall()
    local ball = world.createSphere(Vector(math.random()*0.1,5,math.random()*0.1), 1, 1)
    world.setRestitution(ball, 0.9)
    table.insert(balls, ball)
end

addBall()

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

    if GetMouse(MOUSE_RIGHT_BUTTON) then
        ShowCursor(false)
        SetCursorMode(CURSORMODE_CENTERED)
    else
        ShowCursor(true)
        SetCursorMode(CURSORMODE_DEFAULT)
    end

    if GetKey(KEY_SPACE) then
        addBall()
    end

    world:update()
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

  for _, handle in pairs(balls) do
    sphere:draw(
        world.getWorldTransform(handle)
    )
  end
end
