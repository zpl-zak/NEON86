local ecs = require "ecs"
local e1 = ecs.ent()

local function drawable(modelName)
  return ecs.cmp("drawable", {
    model = Model(modelName)
  })
end

local function spatial(mat)
  return ecs.cmp("spatial", {
    mat=mat
  })
end

e1:add(spatial(Matrix():translate(0,0,5)))
e1:add(drawable("sphere.fbx"))
e1:add(ecs.cmp("spinner", {
  val=0
}))

local draw = ecs.sys({"drawable"}, function (e)
  local mat = Matrix()

  if e.spatial then
    mat=e.spatial.mat
  end

  e.drawable.model:draw(mat)
end)

local spin = ecs.sys({"spatial", "spinner"}, function (e, dt)
  e.spatial.mat = Matrix():rotate(e.spinner.val,0,0):translate(0,0,5)
  e.spinner.val = e.spinner.val + dt
end)

function _update(dt)
  spin({e1}, dt)
end

function _render()
  ClearScene(0,0,0)
  CameraPerspective(62, 0.1, 100)
  Matrix():lookAt(
    Vector3(0,0,-5),
    Vector3(0,0,0),
    Vector3(0,1,0)
  ):bind(VIEW)

  draw({e1})
end
