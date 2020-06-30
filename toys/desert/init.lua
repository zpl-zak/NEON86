local time = 0
local light
local testFont

local cols = require "collisions"
local cam = require "camera"

local camera
local world
local terrain, terrainWire, trMesh, trDebugMesh

function _init()
  world = cols.newWorld()
  terrain = Scene("desert.fbx")
  local terrainRoot = terrain:getRootNode()
  local meshNode = terrainRoot:findNode("terrain")
  local mesh = meshNode:getMeshParts()[1][1]
  trMesh = cols.newTriangleMeshFromPart(mesh, meshNode:getFinalTransform())
  world:addCollision(trMesh)

  trDebugMesh = FaceGroup()

  for _, tr in pairs(trMesh.tris) do
    for i=1,3 do
      local pos = tr[i]:get()
      trDebugMesh:addVertex(Vertex(pos[1], pos[2], pos[3]))
    end
  end
  
  trDebugMesh:build()
  trDebugMesh:calcNormals()

  camera = cam.newCamera(terrainRoot:findTarget("cament"):row(4))
  camera.speed = 30.0
  camera.updateMovement = function(self, dt)
    self.dirs.fwd:y(0) -- Ensure heading doesn't affect movement
    self.pos = self.pos + self.vel
    world:forEach(function (shape)
      ok, move, cp = shape:testSphere(self.pos, 5, Vector(0, self.vel:y(), 0))

      if ok then
        self.pos:y(cp:y() + 5)
        self.vel:y(0)
      end
    end)
    self.vel:y(self.vel:y() - 6*dt)
    self.vel = self.vel + self.vel:neg()*0.10
  end

  light = Light()
  light:setDirection(Vector(-1,-1,1))
  light:setType(LIGHTKIND_DIRECTIONAL)
  light:enable(true, 0)
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
    camera.vel:y(camera.vel:y() + 5)
  end

  ShowCursor(false)
  SetCursorMode(CURSORMODE_CENTERED)

  time = time + dt
end

function _render()
  ClearScene(80,80,69)
  AmbientColor(45,45,45)
  CameraPerspective(62, 0.1, 1000)
  camera.mat:bind(VIEW)
  terrain:draw()

  ToggleWireframe(true)
  EnableLighting(false)
  Matrix():bind(WORLD)
  trDebugMesh:draw()
  EnableLighting(true)
  ToggleWireframe(false)
end

function _render2d()
  testFont:drawText(0xFFFFFFFF, [[
FPS camera with collisions
WASD to move around
SPACE to jump
  ]], 15, 30)
end
