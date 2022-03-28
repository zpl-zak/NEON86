-- Minimal NEON86 game template

testFont = Font("Times New Roman", 36, 2000, false)
sphere = Model("sphere.fbx")
bgColor = Color(20,20,69)
toggleLighting = true
light = Light()
mat = Matrix()
proj = Matrix():persp(62, 0.1, 100)

nicknameTex = nil
nicknameWidth = 200
nicknameHeight = 40
nicknameMeasuredWidth = 0

light:setDirection(Vector(-1,-1,1))
light:enable(true, 0)

cam = require "camera"(Vector(0,0,-5))

require "helpers".global()

function _fixedUpdate(dt)
  if GetKeyDown(KEY_ESCAPE) then
    ExitGame()
  end

  if GetKeyDown("l") then
    toggleLighting = not toggleLighting
  end

  if GetMouse(MOUSE_RIGHT_BUTTON) then
    ShowCursor(false)
    SetCursorMode(CURSORMODE_CENTERED)
  else
      ShowCursor(true)
    SetCursorMode(CURSORMODE_DEFAULT)
  end

  cam:update(dt)
end

function _render()
  ClearScene(bgColor)
  AmbientColor(bgColor)
  EnableLighting(toggleLighting)
  proj:bind(PROJ)
  cam.mat:bind(VIEW)

  mat = Matrix():rotate(getTime()/4,0,0):translate(0,math.sin(getTime()),0)
  sphere:draw(mat)
end

function _render2d()
  testFont:drawText(0xFFFFFFFF, [[
    World-To-Screen text rendering
  ]], 0, 30, 0, 0, FF_NOCLIP)

  if nicknameTex == nil then
    nicknameTex = RenderTarget(200,40, RTKIND_COLOR16)
    nicknameTex:bind()
    ClearScene(0x550000)
    nicknameMeasuredWidth = testFont:measureText("Nickname", FF_SINGLELINE|FF_CENTER, nicknameWidth)[1]
    testFont:drawText(0xFFFFFFFF, "Nickname", 0, 0, nicknameWidth, nicknameHeight, FF_SINGLELINE|FF_CENTER)
    ClearTarget()
  end

  local tag3DPos = mat:translate(0,1.4,0):row(4)
  local tagPos = WorldToScreen(tag3DPos, cam.mat, proj)

  if tagPos:z() < 1 then
    local dist = (cam.pos - tag3DPos):mag() * 0.2
    tagPos:x(tagPos:x() - nicknameMeasuredWidth/2/dist)
    withTexture(nicknameTex, function ()
      DrawQuadEx(tagPos, nicknameWidth/dist, nicknameHeight/dist, 0xFFFFFFFF, true)
    end)
  end
end
