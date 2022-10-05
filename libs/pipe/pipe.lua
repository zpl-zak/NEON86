local Class = require "class"
local helpers = require "helpers"

Class "Pipe" {
  __init__ = function (self)
    self.draws = {}
    self.sorting = false
  end,

  push = function (self, mesh, mat)
    if type(mesh) == "userdata" then
      if mesh.__name == "FaceGroup" then
        table.insert(self.draws, {mesh, mat})
      elseif mesh.__name == "Mesh" then
        self:push(mesh:getParts(), mat)
      elseif mesh.__name == "Node" then
        mat = mat or Matrix()
        self:push(mesh:getMeshes(), mesh:getFinalTransform() * mat)
      elseif mesh.__name == "Scene" then
        self:push(mesh:getFlattenNodes(), mat)
      else
        LogString("Pipe:push() - must be a FaceGroup, Mesh, Node or Scene")
      end
    elseif type(mesh) == "table" then
      for _, v in pairs(mesh) do
        self:push(v, mat)
      end
    end
  end,

  draw = function (self)
    if self.sorting then
      --@todo
    end

    for _, v in pairs(self.draws) do
      v[1]:draw(v[2])
    end

    self.draws = {}
  end,

  sorting = function (self, enable)
    self.sorting = enable
  end,
}

return Pipe
