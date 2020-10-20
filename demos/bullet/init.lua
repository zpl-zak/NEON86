local ecs = require "ecs"
local physWorld = require "bullet"
local Player = require "player"
local helpers = require "helpers"

local ents = {}
local player
local light
local debugMat
local bgColor = Color(60, 60, 69)
local ambColor = Color(40, 40, 69)
local showWireframe = false

LogString("hello")

local MODELS = {
  ball = Model("sphere.fbx"),
  map = Model("bowl.fbx")
}

-- components
local function drawable(modelName)
  return ecs.cmp("drawable", {
    model = function () return MODELS[modelName] end
  })
end

local function spatial(mat)
  return ecs.cmp("spatial", {
    mat = mat
  })
end

local function prop(body)
  return ecs.cmp("prop", {
    body = body
  })
end

local function cols(mat, mesh)
  return ecs.cmp("mesh", {
    body = physWorld.createMesh(mat, mesh)
  })
end

-- factory
local function ball(pos)
  local e = ecs.ent()
  local m = Matrix():translate(pos)
  e:add(spatial(m))
  e:add(drawable("ball"))
  e:add(prop(physWorld.createSphere(m, 1.0, 1.0)))
  table.insert(ents, e)
  return e
end

local function loadScene()
  local sceneNode = MODELS.map:getRootNode():findNode("Cube")
  local e = ecs.ent()
  e:add(drawable("map"))
  e:add(cols(sceneNode:getFinalTransform(), sceneNode:getMeshes()[1]))
  table.insert(ents, e)
end

-- systems
local draw = ecs.sys({"drawable"}, function (e)
  local mat = Matrix()

  if e.spatial then
    mat = e.spatial.mat
  end

  e.drawable.model():draw(mat)
end)

local phys = ecs.sys({"spatial", "prop"}, function (e)
  e.spatial.mat = physWorld.getWorldTransform(e.prop.body)
end)

-- events
function _init()
  loadScene()
  ball(Vector3(0, 25, 0))
  player = Player(physWorld, MODELS.map:getRootNode():findNode("Camera"):getFinalTransform():row(4))

  light = Light()
  light:setDirection(Vector(-0.5,-0.5,1))
  light:enable(true, 0)

  debugMat = Material()
  debugMat:setDiffuse(0,255,0)
  debugMat:setEmission(0,255,0)
end

local function input()
  if GetKeyDown(KEY_ESCAPE) then
    ExitGame()
  end

  if GetMouseDown(MOUSE_LEFT_BUTTON) then
    local handle = ball(player.pos + player.dirs.fwd*2).prop.body
    physWorld.addImpulse(handle, player.dirs.fwd*20)
  end

  if GetKey(KEY_SPACE) then
    ball(Vector(math.random()*0.1,25,math.random()*0.1))
  end

  if GetKeyDown("g") then
    showWireframe = not showWireframe
  end

  ShowCursor(false)
  SetCursorMode(CURSORMODE_CENTERED)
end

function _update(dt)
  input()
  physWorld.update()
  player:update(dt)
  player:update2(dt)
  phys(ents)
end

function _render()
  ClearScene(bgColor)
  AmbientColor(ambColor)
  EnableLighting(true)
  CameraPerspective(62, 0.1, 100)
  player.mat:bind(VIEW)
  draw(ents)

  if showWireframe then
    helpers.withTexture(debugMat, function ()
      ToggleWireframe(true)
      draw(ents)
      ToggleWireframe(false)
    end)
  end
end

local testFont = Font("Arial", 33, 2000, false)

function _render2d()
  testFont:drawText(0xFFFFFFFF, [[
    WASD to move around
    LMB to shoot balls
    RMB to teleport at aimed location
    SPACE for waterfall
    G to toggle wireframe mode
    ]], 15, 30)
end
