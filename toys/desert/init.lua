local time = 0
local light
local testFont

local cols = require "collisions"
local cam = require "camera"

local camera
local world, propCols
local terrain, trMesh, propsNode

function _init()
  world = cols.newWorld()
  propCols = cols.newWorld()
  terrain = Scene("desert.fbx")
  local terrainRoot = terrain:getRootNode()
  propsNode = terrainRoot:findNode("props")
  local meshNode = terrainRoot:findNode("terrain")
  local mesh = meshNode:getMeshParts()[1][1]
  trMesh = cols.newTriangleMeshFromPart(mesh, meshNode:getFinalTransform())
  world:addCollision(trMesh)

  camera = cam.newCamera(terrainRoot:findTarget("cament"):row(4))
  camera.speed = 30.0
  camera.updateMovement = function(self, dt)
    self.dirs.fwd:y(0) -- Ensure heading doesn't affect movement
    self.vel:y(self.vel:y() - 6*dt)
    world:forEach(function (shape)
      ok, move, cp = shape:testSphere(self.pos, 1, Vector(0, self.vel:y(), 0))
      
      if ok then
        self.pos:y(cp:y())
        self.vel:y(0)
      end
    end)
    propCols:forEach(function (side)
      ok, move = side:testSphere(self.pos, 1, Vector(self.vel:x(), 0, 0))
      
      if ok then
        self.vel:x(0)
      end
      
      ok, move = side:testSphere(self.pos, 1, Vector(0, self.vel:y(), 0))
      
      if ok then
        self.vel:y(0)
      end
      
      ok, move = side:testSphere(self.pos, 1, Vector(0, 0, self.vel:z()))
      
      if ok then
        self.vel:z(0)
      end
    end)
    self.pos = self.pos + self.vel
    self.vel = self.vel + self.vel:neg()*0.10
  end

  -- Create prop AABB collisions
  for _, propNode in pairs(propsNode:getNodes()) do
    local prop = propNode:getMeshes()[1]
    for _, part in pairs(prop:getParts()) do
      propCols:addCollision(cols.newBoxFromPart(part, propNode:getFinalTransform()))
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
    camera.vel:y(camera.vel:y() + 2)
  end

  ShowCursor(false)
  SetCursorMode(CURSORMODE_CENTERED)

  time = time + dt
end

function _render()
  ClearScene(80,80,69)
  AmbientColor(80,80,132)
  CameraPerspective(62, 0.1, 1000)
  camera.mat:translate(Vector3(0,-5,0)):bind(VIEW)
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
