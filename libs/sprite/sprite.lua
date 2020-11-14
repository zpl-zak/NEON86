local class = require "class"
local tween = require "tween"

class "Sprite" {
  __init__ = function (self, path, dims, texDims, framenum, framesPerRow, framestep, isLooped)
    self.tex = Material(path)
    self.tex:alphaIsTransparency(true)
    self.dims = dims or self.tex:res(1)
    texDims = texDims or self.dims
    self.stepX = self.dims[1] / texDims[1]
    self.stepY = self.dims[2] / texDims[2]
    self.framestep = framestep or 0.15
    isLooped = isLooped or true

    local keytime = 0.0
    local animLayer = tween.Layer()
    local animAction = tween.Action(isLooped)
    framenum = framenum or 1
    self.framePitch = framesPerRow or framenum

    for i = 0, framenum-1 do
      local pose = tween.FramePose():withProp(i)
      local keyframe = tween.Keyframe(keytime, pose, "none")
      animLayer:add(keyframe)
      keytime = keytime + self.framestep
    end

    animAction:add("frame", animLayer)
    self.anim = tween.Tween()
    self.anim:play(animAction)
  end,

  update = function (self, dt)
    self.anim:update(dt)
  end,

  draw = function (self, idx)
    idx = idx or math.floor(self.anim:getPose("frame"):getProp())
    local x = math.floor(idx % self.framePitch)
    local y = math.floor(idx / self.framePitch)

    local u1 = self.stepX*x
    local v1 = self.stepY*y + self.stepY

    local u2 = u1 + self.stepX
    local v2 = v1 - self.stepY

    BindTexture(0, self.tex)
    DrawPolygon(
      Vertex(0.0, 0.0, 0.0, u1, v1),
      Vertex(0.0, 1.0, 0.0, u1, v2),
      Vertex(1.0, 1.0, 0.0, u2, v2)
    )
    DrawPolygon(
      Vertex(1.0, 1.0, 0.0, u2, v2),
      Vertex(1.0, 0.0, 0.0, u2, v1),
      Vertex(0.0, 0.0, 0.0, u1, v1)
    )
    BindTexture(0)
  end
}

return {
  Sprite=Sprite
}
