local sphere
local sphereModel
local time = 0
local light
local testFont
local roomScene
local room
local cols = require "collisions"
local Camera = require "camera"
local gravity = -0.0981

local DEFAULT_BOUNCE_FACTOR = 1.8
local DEFAULT_SPHERE_VEL_CAP = 1
local bounceFactor = DEFAULT_BOUNCE_FACTOR

local balls = {}
local camera
local world

function _init()
  world = cols.newWorld()
  sphereModel = Model("sphere.fbx", false)
  sphere = sphereModel:getRootNode()

  roomScene = Scene("scene.fbx")
  roomRoot = roomScene:getRootNode()
  room = roomRoot:findNode("room")
  addBall()

  local cament = roomScene:findTarget("campos")
  local campos = cament:row(4)
  local camdir = cament:row(2)
  local camup = cament:row(3):neg()

  camera = Camera(campos)
  camera.movement = function(self, dt)
    self.vel = self.movedir
    world:forEach(function (shape)
      shape:testSphere(self.pos, 1, self.vel, function (norm)
        norm = norm:normalize()
        local wallDir = norm * (self.vel * norm)
        self.vel = self.vel - wallDir
      end)
    end)
    self.pos = self.pos + self.vel
  end

  Matrix():lookAt(
      campos,
      campos+camdir,
      camup
  ):bind(VIEW)

  for _, sideNode in pairs(room:getNodes()) do
    local mat = sideNode:getFinalTransform()
    local part = sideNode:getMeshParts()[1][1]
    local dims = cols.newTriangleMeshFromPart(part, mat)
    world:addCollision(dims)
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
  camera.mat:bind(VIEW)

  if GetKeyDown(KEY_ESCAPE) then
      ExitGame()
  end

  if GetMouse(MOUSE_RIGHT_BUTTON) then
      ShowCursor(false)
    SetCursorMode(CURSORMODE_CENTERED)
  else
      ShowCursor(true)
    SetCursorMode(CURSORMODE_DEFAULT)
  end

  if GetKey(KEY_SPACE) then
      addBall()
  end

  if GetKey("r") then
    if #balls > 0 then
      table.remove(balls, 1)
    end
  end

  if GetKeyDown("m") then
      bounceFactor = 2.35
  end

  if GetKeyDown("b") then
      bounceFactor = 1.2
  end

  if GetKeyDown("n") then
      bounceFactor = DEFAULT_BOUNCE_FACTOR
  end

  if GetKeyDown("t") then
      balls = {}
      addBall()
  end

  time = time + dt
  updateBalls(dt)
end

function _render()
  ClearScene(20,20,69)
  AmbientColor(45,45,45)
  CameraPerspective(62, 0.1, 100)

  room:draw()

  for _, ball in pairs(balls) do
      sphere:draw(Matrix():translate(ball.pos))
  end
end

function _render2d()
  testFont:drawText(0xFFFFFFFF, [[
Simple sphere physics
Press SPACE to spawn another ball!
Hold RMB to look around
M - bounce: crazy
N - bounce: normal
B - bounce: less bouncy
T - reset scene
  ]], 15, 30)
end

function addBall()
  local ovel = Vector3(
      math.random(-DEFAULT_SPHERE_VEL_CAP,DEFAULT_SPHERE_VEL_CAP),
      math.random(-DEFAULT_SPHERE_VEL_CAP,DEFAULT_SPHERE_VEL_CAP),
      math.random(-DEFAULT_SPHERE_VEL_CAP,DEFAULT_SPHERE_VEL_CAP)
  ) * 0.10

  table.insert(balls, {
--         pos = opos,
      vel = ovel,
      pos = Vector3(0,8,0),
--         vel = Vector3(0.04,0,0)
  })
end

function updateBalls(dt)
  for _, ball in pairs(balls) do

      world:forEach(function (shape)
        shape:testSphere(ball.pos, 2, ball.vel, function (norm)
          norm = norm:normalize()
          local wallDir = norm * (ball.vel * norm)
          ball.vel = ball.vel - wallDir*bounceFactor
        end)
      end)

      ball.pos = ball.pos + ball.vel
      ball.vel:y(ball.vel:y()+gravity*dt)
  end
end
