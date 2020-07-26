time = 0

shadowMapSize = 4096.0

cam = require "camera" (Vector(1,2,3)*4, {math.pi, -0.5})
shadowGen = require "shadow" (shadowMapSize)
require "helpers"


lightDir = Vector3(-0.33,-1,1)
scene = Scene("scene.fbx")
sceneNodes = scene:getFlattenNodes()

light = Light()
light:setDirection(lightDir)
light:setDiffuse(255,200,60)
light:setType(LIGHTKIND_DIRECTIONAL)
light:enable(true, 0)
EnableLighting(true)

shader = Effect("fx/main.fx")

RegisterFontFile("slkscr.ttf")
testFont = Font("Silkscreen", 24, 1, false)

shadowMethod = 0
noShadows = 0

function _update(dt)
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

  if GetKeyDown("p") then
    shadowMethod = (shadowMethod + 1) % 3
  end

  if GetKeyDown("m") then
    if shadowMapSize == 256.0 then
      shadowMapSize = 4096.0
    else
      shadowMapSize = 256.0
    end
    shadowGen = ShadowGen(shadowMapSize)
  end

  if GetKeyDown("n") then
    noShadows = 1 - noShadows
  end

  cam:update(dt)

  time = time + dt
end

function drawScene()
  scene:draw()
end

function _render()
  -- 1st pass, generate shadowmap
  local ldir = lightDir * Matrix():rotate(time/4,0,0)
  lightProj = Matrix():orthoEx(-20, 20, -20, 20, -20, 20)
  lightView = Matrix():lookAt(
    ldir,
    Vector(),
    Vector(0,1,0)
  )
  light:setDirection(ldir)
  light:enable(true, 0)

  shadowGen:build(lightView, lightProj, drawScene)

  -- 2nd pass, render scene w/ shadows
  ClearTarget()
  ClearScene(80,80,60)
  AmbientColor(20,20,69)
  CameraPerspective(62, 0.1, 100)
  cam.mat:bind(VIEW)

  drawEffect(shader, "Scene", function (fx)
    fx:setLight("sun", light)
    fx:setTexture("shadowTex", shadowGen.shadowmap)
    fx:setMatrix("shadowView", lightView)
    fx:setMatrix("shadowProj", lightProj)
    fx:setVector3("campos", cam.pos)
    fx:setFloat("shadowMapSize", shadowMapSize)
    fx:setFloat("shadowMethod", shadowMethod)
    fx:setFloat("noShadows", noShadows)
    fx:commit()
    drawScene()
  end)
end

function _render2d()
  testFont:drawText(0xFFFFFFFF, [[
Shadowmap demo

Press P to cycle between shadow techniques
Press M to switch between 256x256 and 4096x4096 resolution

Hold RMB to look around
WASD to move

Techniques available:
  PCF 2x2
  Variance
  A/B Test
  ]], 15, 30)
end
