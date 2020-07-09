local time = 0
local light
local testFont

local cols = require "collisions"
local cam = require "camera"
local hh = require "helpers"

local camera
local world
local terrain, trMesh, propsNode

function _init()
  world = cols.newWorld()
  terrain = Scene("desert.fbx")
  local terrainRoot = terrain:getRootNode()
  propsNode = terrainRoot:findNode("props")
  local meshNode = terrainRoot:findNode("terrain")
  local mesh = meshNode:getMeshParts()[1][1]
  trMesh = cols.newTriangleMeshFromPart(mesh, meshNode:getFinalTransform())
  trMesh.friction = 0.075
  world:addCollision(trMesh)

  camera = cam.newCamera(terrainRoot:findTarget("cament"):row(4))
  camera.speed = 30.0
  camera.updateMovement = function(self, dt)
    if self.grounded == false then
      self.vel:y(self.vel:y() - 6*dt)
    end
    self.movedir:y(0)
    local nvel = Vector()
    local contacts = {}
    world:forEach(function (shape)
      c = shape:testSphere(self.pos, 1, self.movedir + self.vel + Vector3(0,-5,0), function (norm)
        local o = cols.slide((self.vel), norm)
        local g = cols.slide((self.movedir), norm)
        self.grounded = true
        self.vel = hh.lerp(o, g, shape.friction)
      end)
    end)

    self.pos = self.pos + self.vel
  end

  -- Create prop AABB collisions
  for _, propNode in pairs(propsNode:getNodes()) do
    local prop = propNode:getMeshes()[1]
    for _, part in pairs(prop:getParts()) do
      slMesh = cols.newTriangleMeshFromPart(part, propNode:getFinalTransform())
      slMesh.friction = 0.6
      world:addCollision(slMesh)
    end
  end

  light = Light()
  light:setDirection(Vector(-1,-0.2,1))
  light:setType(LIGHTKIND_DIRECTIONAL)
  light:setDiffuse(255,128,0)
  light:enable(true, 0)
  SetFog(VectorRGBA(80,80,69), FOGKIND_EXP, 0.008)
  EnableLighting(true)

  RegisterFontFile("slkscr.ttf")
  testFont = Font("Silkscreen", 18, 1, false)
  
  ShowCursor(false)
  SetCursorMode(CURSORMODE_CENTERED)
end

function _update(dt)
  camera:update(dt)
  
  if GetKeyDown(KEY_ESCAPE) then
    ExitGame()
  end
  
  if GetKeyDown(KEY_SPACE) then
    if camera.grounded == true then
      camera.vel:y(camera.vel:y() + 96*dt)
    end
  end

  time = time + dt
  camera.grounded = false
end

function _render()
  ClearScene(80,80,69)
  AmbientColor(80,80,132)
  CameraPerspective(62, 0.1, 1000)
  camera.mat:bind(VIEW)
  terrain:draw()
  propsNode:draw()
end

function _render2d()
  testFont:drawText(0xFFFFFFFF, [[
FPS camera with collisions
WASD to move around
SPACE to jump
  ]], 15, 30)
end
