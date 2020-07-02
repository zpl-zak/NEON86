local time = 0
local light
local testFont

local cols = require "collisions"
local cam = require "camera"

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
  trMesh.friction = 0.1
  world:addCollision(trMesh)

  camera = cam.newCamera(terrainRoot:findTarget("cament"):row(4))
  camera.speed = 30.0
  camera.updateMovement = function(self, dt)
    self.dirs.fwd:y(0) -- Ensure heading doesn't affect movement
    
    if self.grounded == false then
      self.vel:y(self.vel:y() - 6*dt)
    end
    world:forEach(function (shape)
      shape:testSphere(self.pos, 2, self.vel - Vector3(0,5,0), function (norm)
        local wallDir = norm * (self.vel * norm)
        self.vel = self.vel - wallDir
        self.grounded = true
      end)
    end)
    self.pos = self.pos + self.vel
    self.vel = self.vel + self.vel:neg()*0.10
  end

  -- Create prop AABB collisions
  for _, propNode in pairs(propsNode:getNodes()) do
    local prop = propNode:getMeshes()[1]
    for _, part in pairs(prop:getParts()) do
      slMesh = cols.newTriangleMeshFromPart(part, propNode:getFinalTransform())
      slMesh.friction = 1.0
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
end

function _update(dt)
  camera:update(dt)
  
  if GetKeyDown(KEY_ESCAPE) then
    ExitGame()
  end
  
  if GetKeyDown(KEY_SPACE) then
    if camera.grounded == true then
      camera.vel:y(camera.vel:y() + 3)
    end
  end
  
  ShowCursor(false)
  SetCursorMode(CURSORMODE_CENTERED)

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
