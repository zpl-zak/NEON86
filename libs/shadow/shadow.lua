local Class = require "class"

Class "ShadowGen" {
  __init__ = function (self, shadowMapSize)
    self.shadowmap = RenderTarget(shadowMapSize, shadowMapSize, RTKIND_DEPTH)
    self.shader = Effect("")
    self:clear()
  end,

  build = function (self, view, proj, drawfn)
    oldView = GetMatrix(VIEW)
    oldProj = GetMatrix(PROJ)

    view:bind(VIEW)
    proj:bind(PROJ)

    self.shadowmap:bind()
    CullMode(CULLKIND_CW)

    self.shader:begin("ShadowGather")
    self.shader:beginPass(1)
    drawfn()
    self.shader:endPass()
    self.shader:finish()

    oldView:bind(VIEW)
    oldProj:bind(PROJ)

    -- TODO: Restore old RT
    ClearTarget()
    CullMode(CULLKIND_CCW)
  end,

  clear = function (self)
    self.shadowmap:bind()
    ClearScene(0xFFFFFFFF)
    ClearTarget()
  end,
}

Class "ShadowDraw" {
  __init__ = function (self, shadowMapSize, generator)
    self.shadowMapSize = shadowMapSize or 512
    self.generator = generator or ShadowGen(self.shadowMapSize)
    self.shadows = {}
  end,

  bind = function (self, fx, idx)
    local shadow = self.shadows[idx or 1]

    if shadow == nil then
      return
    end
    fx:setMatrix("shadow.view", shadow[1])
    fx:setMatrix("shadow.proj", shadow[2])
    fx:setTexture("shadowTex", self.generator.shadowmap)
    fx:setFloat("shadowMapSize", self.shadowMapSize)
  end,

  clear = function (self)
    self.generator:clear()
    self.shadows = {}
  end,

  addShadow = function (self, view, proj, drawfn)
    table.insert(self.shadows, {view, proj})
    self.generator:build(view, proj, drawfn)
  end,
}

return {
  ShadowGen = ShadowGen,
  ShadowDraw = ShadowDraw
}
