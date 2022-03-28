local ecs = require "ecs"

local MODELS = {
  sphere = Model("sphere.fbx")
}

local function drawable(modelName)
  return ecs.cmp("drawable", {
    model = function () return MODELS[modelName] end
  })
end

local function spatial(mat)
  return ecs.cmp("spatial", {
    mat=mat
  })
end

local world = {}

for i = -5, 5 do
  local e = ecs.ent()
  e:add(drawable("sphere"))
  e:add(spatial(Matrix():translate(i*2.5, 0, 5)))
  e:add(ecs.cmp("spinner", { val=0 }))
  table.insert(world, e)
end

local draw = ecs.sys({"drawable"}, function (e)
  local mat = Matrix()

  if e.spatial then
    mat=e.spatial.mat
  end

  if e.spinner then
    mat=Matrix():rotate(e.spinner.val,0,0) * mat
  end

  e.drawable.model():draw(mat)
end)

local spin = ecs.sys({"spatial", "spinner"}, function (e, dt)
  e.spinner.val = e.spinner.val + dt
end)

function _fixedUpdate(dt)
  spin(world, dt)
end

function _render()
  ClearScene(0,0,0)
  CameraPerspective(62, 0.1, 100)
  Matrix():lookAt(
    Vector3(0,0,-5),
    Vector3(0,0,0),
    Vector3(0,1,0)
  ):bind(VIEW)

  draw(world)
end
