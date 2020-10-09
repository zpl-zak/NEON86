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
    self.body = world.createCapsule(Matrix():translate(pos), 1, 2, 1.5)
    self.shift = false
    world.setActivationState(self.body, world.DISABLE_DEACTIVATION)
    world.setDamping(self.body, 0.9, 1)
    world.setFriction(self.body, 0.9)
    world.setAngularFactor(self.body, Vector(0,1,0))

    local hit, dpos = world.rayTest(pos, Vector(0,-1,0)*10)

    if hit then
      self:setPos(dpos+Vector3(0,2,0))
    end
  end,

  movement = function (self, dt)
    if self.shift then
      local dpos = helpers.lerp(self.pos, self.shiftEnd, self.shiftValue)
      dpos:y(dpos:y() + math.cos(self.shiftValue)*0.25)
      self:setPos(dpos)
      self.shiftValue = self.shiftValue + 0.01

      if (self.pos-self.shiftEnd):magSq() < 1 then
        self.shift = false
      end
    else
      local hvel = world.getVelocity(self.body)
      self.vel = helpers.lerp(self.vel, self.movedir, 0.4)
      self.vel:y(hvel:y())
      -- world.addImpulse(self.body, self.movedir)
      world.setVelocity(self.body, self.vel)
    end
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

    if GetMouseDown(MOUSE_RIGHT_BUTTON) then
      local handle, dpos = world.rayTest(self.pos, self.dirs.fwd * 100)
      if handle ~= nil then
        self.shiftEnd = dpos+Vector(0,2,0)
        self.shift = true
        self.shiftValue = 0
      end
    end

    if GetMouseDown(MOUSE_LEFT_BUTTON) then
      local handle = addBall(Matrix():translate(self.pos + self.dirs.fwd*2))
      world.addImpulse(handle, self.dirs.fwd*20)
    end
  end,

  setPos = function (self, pos)
    self.pos = pos
    world.setWorldTransform(self.body, Matrix():translate(pos))
    world.setVelocity(self.body, Vector())
  end
}

local balls = {}
local ballSize = 1.0
local bowl = Model("bowl.fbx")
local node = bowl:getRootNode():findNode("Cube")
local plane = world.createMesh(node:getFinalTransform(), node:getMeshes()[1])
local cam = GameCamera(bowl:getRootNode():findNode("Camera"):getFinalTransform():row(4))
world.setRestitution(plane, 0.9)

-- for _, mesh in pairs(node:getMeshes()) do
--     for _, part in pairs(mesh:getParts()) do
--         part:getMaterial(1):setSamplerState(SAMPLERSTATE_MAGFILTER, TEXF_POINT)
--         part:getMaterial(1):setSamplerState(SAMPLERSTATE_MINFILTER, TEXF_POINT)
--         part:getMaterial(1):setSamplerState(SAMPLERSTATE_MIPFILTER, TEXF_POINT)
--     end
-- end

function addBall(pos)
  if #balls >= 150 then
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
showWireframe = false

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
    addBall(Matrix():translate(Vector(math.random()*0.1,25,math.random()*0.1)))
  end

  if GetKeyDown("g") then
    showWireframe = not showWireframe
  end


  cam:update2(dt)
  world.update()
  cam:update(dt)
end

local debugMat = Material()
debugMat:setDiffuse(0,255,0)
debugMat:setEmission(0,255,0)

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

  if showWireframe == true then
    helpers.withTexture(debugMat, function ()
      ToggleWireframe(true)
      for i, handle in pairs(balls) do
        sphere:draw(
          Matrix():scale(ballSize, ballSize, ballSize) * world.getWorldTransform(handle)
        )
      end
      bowl:draw()
      ToggleWireframe(false)
    end)
  end
end

local testFont = Font("Arial", 33, 2000, false)

function _render2d()
  testFont:drawText(0xFFFFFFFF, [[
    WASD to move around
    LMB to shoot balls
    RMB to teleport at aimed location
    SPACE for waterfall
    G to toggle wireframe mode
    ]], 15, 30)
  end
