local _ = {}

-- Camera API

local Camera = {}
Camera.__index = Camera

function Camera.updateMatrix(self)
  self.mat = Matrix():translate(self.pos:neg())
                     :rotate(-self.angles[1], 0, 0)
                     :rotate(0, self.angles[2], 0)
  self:updateDirVectors()
end

function Camera.updateDirVectors(self)
  self.dirs = {
    fwd = self.mat:col(3),
    rhs = self.mat:col(1)
  }
end

function Camera.update(self, dt)
  self:updateInput(dt)
  self:updateMouseLook(dt)
  self:updateMatrix()
  self:updateMovement(dt)
end

function Camera.updateInput(self, dt)
	local vel = Vector3()

	if GetKey("w") then
		vel = vel + Vector3(self.dirs.fwd * dt * self.speed)
	end

	if GetKey("s") then
		vel = vel + Vector3(self.dirs.fwd * dt * self.speed):neg()
	end

	if GetKey("a") then
		vel = vel + Vector3(self.dirs.rhs * dt * self.speed):neg()
	end

	if GetKey("d") then
		vel = vel + Vector3(self.dirs.rhs * dt * self.speed)
	end

	if GetKey(KEY_SHIFT) then
		vel = vel * 4
	end

	if vel:mag() ~= 0 then
		self.vel = self.vel + (Vector3(vel) - self.vel)*0.10
	end
end

function Camera.updateMouseLook(self, dt)
  if GetCursorMode() == CURSORMODE_CENTERED then
		mouseDelta = GetMouseDelta()
		self.angles[1] = self.angles[1] + (mouseDelta[1] * dt * self.sensitivity)
		self.angles[2] = self.angles[2] - (mouseDelta[2] * dt * self.sensitivity)
	end
end

function Camera.updateMovement(self)
  self.pos = self.pos + self.vel
  self.vel = self.vel + self.vel:neg()*0.10
end

-- Public API

function _.newCamera(pos, angles)
  if pos == nil then
    pos = Vector3()
  end
  if angles == nil then
    angles = {0.0, 0.0}
  end

  local self = setmetatable({}, Camera)
  self.pos = pos
  self.angles = angles
  self.speed = 15.0
  self.sensitivity = 0.15
  self.vel = Vector3()
  self.heldControls = false
  self:updateMatrix()
  return self
end

return _
