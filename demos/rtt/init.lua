local test
local testModel
local sphere
local skybox
local time = 0
local testFont
local rt
local rtMat
local camera

local Camera = require "camera"

require "helpers".global()

local changeRot = 0

function _init()
  testModel = Model("test.fbx")
  testModel:getRootNode():setTransform(Matrix():scale(2,1,1))
  sphere = Model("sphere.fbx")
  camera = Camera(Vector3(-2,0,-8))
  camera:fixAngle({0.48500002529471, 0})

  skybox = Model("skybox.fbx")
  skybox:getMeshes()[1]:getMaterial(1):setShaded(false)

  RegisterFontFile("slkscr.ttf")
  testFont = Font("Silkscreen", 18, 1, false)

  local res = GetResolution()
  rt = RenderTarget(2048, 2048)
  rtMat = Material()
  rtMat:setHandle(1, rt:getHandle())

  ShowCursor(false)
SetCursorMode(CURSORMODE_CENTERED)
end

function _fixedUpdate(dt)
  if GetKeyDown(KEY_ESCAPE) then
      ExitGame()
  end

  if GetKeyDown(KEY_F2) then
  ShowCursor(not IsCursorVisible())
  SetCursorMode(1-GetCursorMode())
  end

  if GetKeyDown("m") then
      changeRot = 1 - changeRot
  end
end

function _update(dt)
  camera:update(dt)
  time = time + dt
end

function _render()
  rt:bind()
  CameraPerspective(62.5, 0.1, 500, true)
  camera.mat:bind(VIEW)

  skybox:draw(Matrix():translate(camera.pos))
  withTexture(rt, function()
    sphere:draw(Matrix():translate(2 +math.cos(time)*1.2,0,-4))
  end)
  testModel:getMeshes()[1]:setMaterial(rtMat)
  sphere:getMeshes()[1]:setMaterial(rtMat)
  testModel:draw()

  ClearTarget()
  CameraPerspective(62, 0.1, 500)
  rt:bind()
  CullMode(3)
  EnableLighting(false)
  ClearScene(20,20,69)

  skybox:draw(Matrix():translate(camera.pos))
  withTexture(rt, function()
    sphere:draw(Matrix():translate(2 +math.cos(time)*1.2,0,-4))
  end)
  testModel:draw()

  ClearTarget()
  ClearScene(120,20,69)
  skybox:draw(Matrix():translate(camera.pos))

  withTexture(rt, function()
    sphere:draw(Matrix():translate(2 +math.cos(time)*1.2,0,-4))
  end)
  testModel:draw()
end

function _render2d()
  testFont:drawText(0xFFFFFFFF, "RTT test (no shaders)\nWASD to move around\nM to toggle mirror rotation", 15, 30)
end
