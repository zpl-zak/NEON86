time = 0

shadowMapSize = 4096.0
debugVis = false

cam = require "camera" (Vector(1,2,3)*4, {math.pi, -0.5})
shadowGen = require "shadow" (shadowMapSize)
require "helpers"

res = GetResolution()

debugView = RenderTarget(math.floor(res[1]/4), math.floor(res[2]/4))
debugViewMini = RenderTarget(math.floor(res[1]/4), math.floor(res[2]/4))
debugViewUnlit = RenderTarget(math.floor(res[1]/4), math.floor(res[2]/4))
debugViewDepth = RenderTarget(math.floor(res[1]/4), math.floor(res[2]/4))

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
paused = false

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

  if GetKeyDown("v") then
    debugVis = not debugVis
  end

  if GetKeyDown("i") then
    paused = not paused
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

  if not paused then
    time = time + dt
  end
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
  if debugVis then
    EnableLighting(false)
    debugView:bind()
    ClearScene(0xFF202069)
    lightProj:bind(PROJ)
    Matrix():lookAt(
      ldir,
      Vector(),
      Vector(0,1,0)
    ):bind(VIEW)
    drawScene()

    debugViewMini:bind()
    ClearScene(0xFF692020)
    lightProj:bind(PROJ)
    Matrix():lookAt(
      ldir:neg(),
      Vector(),
      Vector(0,1,0)
    ):bind(VIEW)
    drawScene()

    debugViewUnlit:bind()
    ClearScene(0xFF206920)
    CameraPerspective(31, 0.1, 100)
    cam.mat:bind(VIEW)
    drawScene()

    debugViewDepth:bind()
    CameraPerspective(45, 1, 80)
    cam.mat:bind(VIEW)
    ClearScene(0xFF00FF00)

    withEffect(shadowGen.shader, "Shadow", function ()
      drawScene()
    end)

    EnableLighting(true)
    ClearTarget()

    withTexture(shadowGen.shadowmap, function ()
      DrawQuad(res[1]-512, res[1], 0, 512, 0xFFFFFFFF)
    end)
    withTexture(debugView, function ()
      DrawQuad(res[1]-768, res[1]-512, 0, 256, 0xFFFFFFFF)
    end)
    withTexture(debugViewMini, function ()
      DrawQuad(res[1]-768, res[1]-512, 256, 512, 0xFFFFFFFF)
    end)
    withTexture(debugViewUnlit, function ()
      DrawQuad(res[1]-1024, res[1]-768, 0, 256, 0xFFFFFFFF)
    end)
    withTexture(debugViewDepth, function ()
      DrawQuad(res[1]-1024, res[1]-768, 256, 512, 0xFFFFFFFF)
    end)
  end
end

function _render2d()
  testFont:drawText(0xFFFFFFFF, [[
    Shadowmap demo

    Press P to cycle between shadow techniques
    Press M to switch between 256x256 and 4096x4096 resolution
    Press V to toggle debug render
    Press I to pause light rotation

    Hold RMB to look around
    WASD to move

    Techniques available:
    PCF 2x2
    Variance
    A/B Test
    ]], 15, 30)
end
