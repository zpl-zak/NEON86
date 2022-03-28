-- Skeletal anims demo

testFont = Font("Times New Roman", 36, 2000, false)
sphere = Model("sphere.fbx")
bgColor = Color(20,20,69)
slowMode = false
light = Light()

light:setDirection(Vector(-1,-1,1))
light:enable(true, 0)

viewMat = Matrix():lookAt(
  Vector(0,0,-5),
  Vector(),
  Vector(0,1,0)
)

local seq = require "tween"

local frame1 = seq.Keyframe(0, seq.FramePose():withRot(Vector(0,0,0)))
local frame2 = seq.Keyframe(4, seq.FramePose():withRot(Vector(math.rad(180),0,0)))
local frame3 = seq.Keyframe(8, seq.FramePose():withRot(Vector(math.rad(360),0,0)))

local layerBase = seq.Layer()
layerBase:add(frame1)
layerBase:add(frame2)
layerBase:add(frame3)

local spinAnim = seq.Action(true)
spinAnim:add("base", layerBase)

spinAnim:event(spinAnim.maxtime, function (_, time)
  LogString("Event fired at: "..time)
end)

local animator = seq.Tween()
animator:play(spinAnim)

function _fixedUpdate(dt)
  if GetKeyDown(KEY_ESCAPE) then
    ExitGame()
  end

  if GetKeyDown("l") then
    slowMode = not slowMode
  end

  local speed = 1.0

  if slowMode then speed = 0.5 end

  animator:update(dt*speed)
end

function _render()
  ClearScene(bgColor)
  AmbientColor(bgColor)
  CameraPerspective(62, 0.1, 100)
  EnableLighting(true)
  viewMat:bind(VIEW)

  sphere:draw(animator:getMatrix("base"))
end

function _render2d()
  testFont:drawText(0xFFFFFFFF, [[
  Welcome to the anims demo test!

  Press L to toggle slow mode
  ]], 0, 30, 0, 0, FF_NOCLIP)
end
