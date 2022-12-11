local Camera = require "camera"
local Class = require "class"
local helpers = require "helpers"

local gravity = 0.27 -- pseudo gravity
local min_max_y = 1 -- clamp mouse view

Class "Player" (Camera) {
  __init__ = function(self, world, pos, angles)
    Camera:__init__(pos, angles)
    self.grounded = false
    self.world = world -- capsule collider
    self.body = self.world.createCapsule(Matrix():translate(pos), 1, 2, 1.5)
    self.shift = false
    self.world.setActivationState(self.body, self.world.DISABLE_DEACTIVATION)
    self.world.setDamping(self.body, 0.9, 1)
    self.world.setFriction(self.body, 0.9)
    self.world.setAngularFactor(self.body, Vector(0, 1, 0))
    self.speed = 10

    local hit, dpos = self.world.rayTest(pos, Vector(0, -1, 0) * 3)

    if hit then
      self:setPos(dpos + Vector3(0, 2, 0))
    end
  end,
  -- smooth movement according to the helper script
  movement = function(self, dt)
    if self.shift then
      local dpos = helpers.lerp(self.pos, self.shiftEnd, self.shiftValue)
      dpos:y(dpos:y() + math.cos(self.shiftValue) * 2)
      self:setPos(dpos)
      self.shiftValue = self.shiftValue + 0.01

      if (self.pos - self.shiftEnd):magSq() < 1 then
        self.shift = false
      end
    else
      local hvel = self.world.getVelocity(self.body)
      self.vel = helpers.lerp(self.vel, self.movedir, 0.4)
      self.vel:y(hvel:y() - gravity) -- pseudo gravity
      self.world.setVelocity(self.body, self.vel)
    end
  end,
  -- update matrix view
  updateMatrix = function(self)
    self.pos = self.world.getWorldTransform(self.body):row(4)
    local yy = self.angles[2] -- y axis
    -- clamp mouse view
    if (yy > min_max_y) then
      yy = min_max_y
    end

    if (yy < -min_max_y) then
      yy = -min_max_y
    end

    self.mat = Matrix():translate(self.pos:neg() - Vector3(0, 1, 0))
        :rotate(-self.angles[1], 0, 0)
        :rotate(0, yy, 0)
    self:updateDirVectors()
  end,
  -- check ground after update
  checkGround = function(self, dt)
    self.grounded = false
  end,

  setPos = function(self, pos)
    self.pos = pos
    self.world.setWorldTransform(self.body, Matrix():translate(pos))
    self.world.setVelocity(self.body, Vector())
  end
}

return Player
