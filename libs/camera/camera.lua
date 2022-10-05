local class = require "class"
local helpers = require "helpers"

class "Camera" {
  __init__ = function (self, pos, angles)
    if pos == nil then
      pos = Vector3()
    end
    if angles == nil then
    angles = {0.0, 0.0}
    end

    self.pos = pos
    self:fixAngle(angles)
    self.speed = 15.0
    self.accel = 6.0
    self.sensitivity = 0.05
    self.smoothing = 20
    self.vel = Vector3()
    self.heldControls = false
    self:updateMatrix()
  end,

  -- Events

  update = function (self, dt)
    self:input(dt)
    self:mouseLook(dt)
    self:updateMatrix()
    self:movement(dt)
  end,

  lookUpdate = function (self, dt)
    self:mouseLook(dt)
    self:updateMatrix()
  end,

  input = function (self, dt)
    local movedir = Vector3()

    if GetKey("w") then
      movedir = movedir + Vector3(self.dirs.fwd * self.speed)
    end

    if GetKey("s") then
      movedir = movedir + Vector3(self.dirs.fwd * self.speed):neg()
    end

    if GetKey("a") then
      movedir = movedir + Vector3(self.dirs.rhs * self.speed):neg()
    end

    if GetKey("d") then
      movedir = movedir + Vector3(self.dirs.rhs * self.speed)
    end

    if GetKey(KEY_SHIFT) then
      movedir = movedir * 4
    end

    self.movedir = movedir
  end,

  mouseLook = function (self, dt)
    if GetCursorMode() == CURSORMODE_CENTERED then
      mouseDelta = GetMouseDelta()
      self.newAngles[1] = self.newAngles[1] + (mouseDelta[1] * self.sensitivity * dt)
      self.newAngles[2] = self.newAngles[2] - (mouseDelta[2] * self.sensitivity * dt)
    end

    if self.smoothing > 0.0 then
      self.angles[1] = helpers.lerp(self.angles[1], self.newAngles[1], self.smoothing*dt)
      self.angles[2] = helpers.lerp(self.angles[2], self.newAngles[2], self.smoothing*dt)
    else
      self.angles[1] = self.newAngles[1]
      self.angles[2] = self.newAngles[2]
    end
  end,

  movement = function (self, dt)
    self.vel = helpers.lerp(self.vel, self.movedir, self.accel * dt)
    self.pos = self.pos + self.vel*dt
  end,

  -- Helpers

  updateMatrix = function (self)
    self.mat = Matrix():translate(self.pos:neg())
              :rotate(-self.angles[1], 0, 0)
              :rotate(0, self.angles[2], 0)
    self:updateDirVectors()
  end,

  updateDirVectors = function (self)
    self.dirs = {
      fwd = self.mat:col(3),
      rhs = self.mat:col(1)
    }
  end,

  fixAngle = function (self, angles)
    self.newAngles = {table.unpack(angles)}
    self.angles = {table.unpack(angles)}
    self:updateMatrix()
  end
}

return Camera
