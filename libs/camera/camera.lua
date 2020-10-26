local class = require "class"

class "Camera" {
  __init__ = function (self, pos, angles)
    if pos == nil then
      pos = Vector3()
    end
    if angles == nil then
    angles = {0.0, 0.0}
    end

    self.pos = pos
    self.angles = angles
    self.speed = 15.0
    self.sensitivity = 0.15
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

  input = function (self, dt)
    local movedir = Vector3()

    if GetKey("w") then
      movedir = movedir + Vector3(self.dirs.fwd * dt * self.speed)
    end

    if GetKey("s") then
      movedir = movedir + Vector3(self.dirs.fwd * dt * self.speed):neg()
    end

    if GetKey("a") then
      movedir = movedir + Vector3(self.dirs.rhs * dt * self.speed):neg()
    end

    if GetKey("d") then
      movedir = movedir + Vector3(self.dirs.rhs * dt * self.speed)
    end

    if GetKey(KEY_SHIFT) then
      movedir = movedir * 4
    end

    self.movedir = movedir
  end,

  mouseLook = function (self, dt)
    if GetCursorMode() == CURSORMODE_CENTERED then
      mouseDelta = GetMouseDelta()
      self.angles[1] = self.angles[1] + (mouseDelta[1] * dt * self.sensitivity)
      self.angles[2] = self.angles[2] - (mouseDelta[2] * dt * self.sensitivity)
    end
  end,

  movement = function (self)
    self.vel = self.vel + (self.movedir - self.vel)*0.10
    self.pos = self.pos + self.vel
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
}

return Camera
