local sprite = require "sprite"
local helpers = require "helpers"

bgColor = Color(30,20,20)
toggleLighting = true
light = Light()

fire = sprite.Sprite("fire.png", {128, 128}, {512, 128}, 4, nil, 0.1)
fire.tex:setOpacity(0.8)
fire.tex:setEmission(Color(255,255,255))

roomScene = Scene("fire.fbx")
fireSpot = roomScene:findTarget("Spot"):row(4)

lightFX = Effect("main.fx")

useShader = true

light:setType(LIGHTKIND_POINT)
light:setPosition(Vector(0,2,0))
light:setRange(32767)
light:setAttenuation(1.0,0.1,0.01)
light:setDiffuse(Color(120*2,110*2,40*2))
-- light:setSpecular(Color(255,255,255))
light:enable(true, 0)
EnableLighting(true)

viewMat = Matrix():lookAt(
  Vector(0,4,-4),
  Vector(0,1.5,0),
  Vector(0,1,0)
)

fireSFX = Sound("fire.wav")
fireSFX:setVolume(50)
fireSFX:loop(true)
fireSFX:play()

function _fixedUpdate(dt)
  if GetKeyDown(KEY_ESCAPE) then
    ExitGame()
  end

  if GetKeyDown("p") then
    useShader = not useShader
  end

  fire:update(dt)
end

function _render()
  ClearScene(bgColor)
  AmbientColor(bgColor)
  CameraPerspective(62, 0.1, 100)
  local mat = Matrix():rotate(getTime()/2,0,0) * viewMat
  mat:bind(VIEW)

  if useShader then
    helpers.withEffect(lightFX, "Main", function (fx)
      fx:setLight("bonfire", light)
      roomScene:draw()
    end)
  else
    roomScene:draw()
  end

  Matrix()
    :translate(-0.5,-0.3,0)
    :scale(8):translate(fireSpot)
    :translate(0,-2.2,0)
    :rotate(-getTime()/2,0,0)
    :bind(WORLD)

  fire:draw()
  Matrix():bind(WORLD)
end

local testFont = Font("Arial", 72, 1, false)

function _render2d()
  local text = "Shader"
  if not useShader then
    text = "VertexLit Direct3D"
  end
  testFont:drawText(0xFFFFFFFF, text, 15, 30)
end
