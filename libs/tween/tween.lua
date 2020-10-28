local class = require "class"
local helpers = require "helpers"

class "FramePose" {
  __init__ = function (self)
    self.comps = {Vector(), Vector(), Vector(1,1,1), 0}
    self.uses = {false, false, false, false}
  end,

  withPos = function (self, pos)
    self.comps[1] = pos
    self.uses[1] = true
    return self
  end,

  withRot = function (self, rot)
    self.comps[2] = rot
    self.uses[2] = true
    return self
  end,

  withScale = function (self, scale)
    self.comps[3] = scale
    self.uses[3] = true
    return self
  end,

  withProp = function (self, num)
    self.comps[4] = num
    self.uses[4] = true
  end,

  getPos = function (self)
    return self.comps[1]
  end,

  getRot = function (self)
    return self.comps[2]
  end,

  getScale = function (self)
    return self.comps[3]
  end,

  getProp = function (self)
    return self.comps[4]
  end,

  default = function (i)
    if i == 3 then
      return Vector(1,1,1)
    elseif i == 4 then
      return 0
    else
      return Vector()
    end
  end
}

class "Keyframe" {
  __init__ = function (self, time, pose, kind)
    self.time = time
    self.pose = pose
    self.kind = kind or "linear"
  end
}

class "Layer" {
  __init__ = function (self)
    self.keyframes = {}
    self.maxtime = -1.0
    self.pose = FramePose()
  end,

  step = function (self, time)
     for i = 1, #self.pose.comps do
      local base = self:findBase(time, i)
      local goal = self:findGoal(time, i)

      if base == nil or goal == nil then
        if base ~= nil then
          self.pose.comps[i] = base.pose.comps[i]
        else
          self.pose.comps[i] = FramePose.default(i)
        end
      else
        -- TODO Implement other interpolation types
        self.pose.comps[i] = helpers.lerp(base.pose.comps[i], goal.pose.comps[i], (time-base.time) / (goal.time-base.time))
      end
    end

    self.mat = Matrix():scale(self.pose.comps[3]):rotate(self.pose.comps[2]):translate(self.pose.comps[1])
  end,

  add = function (self, keyframe)
    if keyframe.time > self.maxtime then
      table.insert(self.keyframes, keyframe)
      self.maxtime = keyframe.time
      self:updateMaxTime()
    end
  end,

  setAction = function (self, action)
    self.action = action
    self:updateMaxTime()
  end,

  updateMaxTime = function (self)
    if self.action ~= nil then
      if self.action.maxtime < self.maxtime then
        self.action.maxtime = self.maxtime
      end
    end
  end,

  findBase = function (self, time, comp)
    local keyframe = nil

    for _, frame in pairs(self.keyframes) do
      if frame.time <= time and frame.pose.uses[comp] then
        keyframe = frame
      else
        break
      end
    end

    return keyframe
  end,

  findGoal = function (self, time, comp)
    for _, frame in pairs(self.keyframes) do
      if frame.time > time and frame.pose.uses[comp] then
        return frame
      end
    end

    return nil
  end,
}

class "Action" {
  __init__ = function (self, loop)
    self.layers = {}
    self.maxtime = -1
    self.loop = loop or false
    self.events = {}
  end,

  step = function (self, time)
    for _, layer in pairs(self.layers) do
      layer:step(time)
    end

    for _, event in pairs(self.events) do
      if not event.fired and event.time <= time then
        event.fired = true
        event.cb(self, time)
      end
    end

    return self.maxtime <= time
  end,

  add = function (self, name, layer)
    self.layers[name] = layer
    layer:setAction(self)
  end,

  event = function (self, time, cb)
    table.insert(self.events, {
      time = time,
      cb = cb,
      fired = false
    })
  end,

  reset = function (self)
    for _, event in pairs(self.events) do
      event.fired = false
    end
  end
}

class "Tween" {
  __init__ = function (self)
    self.c = nil
    self.time = 0
    self.paused = true
  end,

  update = function (self, dt)
    if self.c == nil or self.paused then return end

    if self.c:step(self.time) then
      if self.c.loop then
        self.time = 0
        self.c:reset()
      else
        self:stop()
      end
    else
      self.time = self.time + dt
    end
  end,

  getPose = function (self, layerName)
    if self.c == nil then return FramePose() end
    if self.c.layers[layerName] == nil then return FramePose() end
    return self.c.layers[layerName].pose
  end,

  getMatrix = function (self, layerName)
    if self.c == nil then return Matrix() end
    if self.c.layers[layerName] == nil then return Matrix() end
    return self.c.layers[layerName].mat
  end,

  play = function (self, action)
    action:reset()
    self.c = action
    self.paused = false
    self.time = 0
  end,

  resume = function (self)
    self.paused = false
  end,

  stop = function (self)
    self.paused = true
  end,

  reset = function (self)
    if self.c ~= nil then self.c:reset() end
    self.time = 0
  end
}

return {
  Tween=Tween,
  Action=Action,
  Layer=Layer,
  Keyframe=Keyframe,
  FramePose=FramePose,
}
