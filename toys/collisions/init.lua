local sphere
local sphereModel
local time = 0
local light
local testFont
local roomScene
local room
local cols = require "collisions"
local cam = require "camera"
local gravity = -0.0981

local DEFAULT_BOUNCE_FACTOR = 0.90
local DEFAULT_SPHERE_VEL_CAP = 0.001
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

  camera = cam.newCamera(campos)
  camera.updateMovement = function(self, dt)
    self.pos = self.pos + self.vel
    world:forEach(function (side)
        ok, move = side:testSphere(self.pos, 1, Vector(self.vel:x(), 0, 0))

        if ok then
            self.pos = self.pos + move
        end

        ok, move = side:testSphere(self.pos, 1, Vector(0, self.vel:y(), 0))

        if ok then
            self.pos = self.pos + move
        end

        ok, move = side:testSphere(self.pos, 1, Vector(0, 0, self.vel:z()))

        if ok then
            self.pos = self.pos + move
        end
    end)
    self.vel = self.vel + self.vel:neg()*0.10
  end

  Matrix():lookAt(
      campos,
      campos+camdir,
      camup
  ):bind(VIEW)

  for _, side in pairs(room:getNodes()) do
      local dims = side:getMeshParts()[1][1]:getBounds()
      local mat = side:getTransform():translate(Vector(0,10,10))
      local tdims = cols.newBox(dims, mat)
      world:addCollision(tdims)
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

  if GetKeyDown(KEY_SPACE) then
      addBall()
  end

  if GetKeyDown("m") then
      bounceFactor = 1.35
  end

  if GetKeyDown("b") then
      bounceFactor = 0.5
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

  ToggleWireframe(true)
  world:forEach(function (side)
      DrawBox(Matrix():translate(side.mat:row(4)), side.dims, 0xFFFFFFFF)
  end)
  ToggleWireframe(false)

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
  )

  table.insert(balls, {
--         pos = opos,
      vel = ovel,
      pos = Vector3(0,8,0),
--         vel = Vector3(0.04,0,0)
  })
end

function updateBalls(dt)
  for _, ball in pairs(balls) do
      local vel = ball.vel:get()

      world:forEach(function (side)
        for i=1,3 do
          local v = Vector():m(i, vel[i])
          ok, delta = side:testSphere(ball.pos, 2, v)

          if ok then
              ball.vel = ball.vel:m(i, vel[i]*-1*bounceFactor)
              ball.pos = ball.pos + delta
              i=99
          end
        end
      end)

      ball.pos = ball.pos + ball.vel
      ball.vel:y(ball.vel:y()+gravity*dt)
  end
end
