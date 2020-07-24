time = 0

shadowMapSize = 4096.0

Camera = require "camera"
Class = require "class"
require "helpers"

Class "ShadowGen" {
  __init__ = function (self, shadowMapSize)
    self.shadowmap = RenderTarget(shadowMapSize, shadowMapSize, true)
    self.shader = Effect("fx/shadowPass.fx")
  end,

  build = function (self, view, proj, drawfn)
    oldView = GetMatrix(VIEW)
    oldProj = GetMatrix(PROJ)

    view:bind(VIEW)
    proj:bind(PROJ)

    self.shadowmap:bind()
    ClearScene(0,0,0)
    CullMode(CULLKIND_CW)

    self.shader:begin("Shadow")
    self.shader:beginPass(1)
    drawfn()
    self.shader:endPass()
    self.shader:finish()

    oldView:bind(VIEW)
    oldProj:bind(PROJ)

    -- TODO: Restore old RT
    ClearTarget()
    CullMode(CULLKIND_CCW)
  end,
}

cam = Camera(Vector(1,2,3)*4, {math.pi, -0.5})

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
shadowGen = ShadowGen(shadowMapSize)

RegisterFontFile("slkscr.ttf")
testFont = Font("Silkscreen", 24, 1, false)

shadowMethod = 0

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
    fx:setFloat("shadowMapSize", shadowMapSize)
    fx:setFloat("shadowMethod", shadowMethod)
    fx:commit()
    drawScene()
  end)
end

function _render2d()
  testFont:drawText(0xFFFFFFFF, [[
Shadowmap demo

Press P to cycle between shadow techniques
Press M to switch between 128x128 and 4096x4096 resolution

Hold RMB to look around
WASD to move

Techniques available:
  PCF 2x2
  Variance
  A/B Test
  ]], 15, 30)
end
