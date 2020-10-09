local Camera = require "camera"
local Class = require "class"
local helpers = require "helpers"

Class "Player" (Camera) {
  -- Override movement method for collisions
  __init__ = function (self, world, pos, angles)
    Camera:__init__(pos, angles)
    self.grounded = false
    self.speed = 500
    self.world = world
    self.body = self.world.createCapsule(Matrix():translate(pos), 1, 2, 1.5)
    self.shift = false
    self.world.setActivationState(self.body, self.world.DISABLE_DEACTIVATION)
    self.world.setDamping(self.body, 0.9, 1)
    self.world.setFriction(self.body, 0.9)
    self.world.setAngularFactor(self.body, Vector(0,1,0))

    local hit, dpos = self.world.rayTest(pos, Vector(0,-1,0)*10)

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
      local hvel = self.world.getVelocity(self.body)
      self.vel = helpers.lerp(self.vel, self.movedir, 0.4)
      self.vel:y(hvel:y())
      -- self.world.addImpulse(self.body, self.movedir)
      self.world.setVelocity(self.body, self.vel)
    end
  end,

  updateMatrix = function (self)
    self.pos = self.world.getWorldTransform(self.body):row(4)
    self.mat = Matrix():translate(self.pos:neg()-Vector3(0,1,0))
              :rotate(-self.angles[1], 0, 0)
              :rotate(0, self.angles[2], 0)
    self:updateDirVectors()
  end,

  update2 = function (self, dt)
    self.grounded = false

    if GetMouseDown(MOUSE_RIGHT_BUTTON) then
      local handle, dpos = self.world.rayTest(self.pos, self.dirs.fwd * 100)
      if handle ~= nil then
        self.shiftEnd = dpos+Vector(0,2,0)
        self.shift = true
        self.shiftValue = 0
      end
    end
  end,

  setPos = function (self, pos)
    self.pos = pos
    self.world.setWorldTransform(self.body, Matrix():translate(pos))
    self.world.setVelocity(self.body, Vector())
  end
}

return Player
