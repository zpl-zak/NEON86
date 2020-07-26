local Class = require "class"

Class "ShadowGen" {
    __init__ = function (self, shadowMapSize)
      self.shadowmap = RenderTarget(shadowMapSize, shadowMapSize, RTKIND_DEPTH)
      self.shader = Effect("fx/shadowPass.fx")
    end,

    build = function (self, view, proj, drawfn)
      oldView = GetMatrix(VIEW)
      oldProj = GetMatrix(PROJ)

      view:bind(VIEW)
      proj:bind(PROJ)

      self.shadowmap:bind()
      ClearScene(0,0,0)
      CullMode(CULLKIND_CW)

      self.shader:begin("Shadow")
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
  }

return ShadowGen