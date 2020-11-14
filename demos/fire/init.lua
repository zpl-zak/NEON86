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

light:setType(LIGHTKIND_POINT)
light:setPosition(Vector(0,2,0))
light:setRange(5)
light:setAttenuation(0.2,1,0.8)
light:setDiffuse(Color(120,110,40))
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

function _update(dt)
  if GetKeyDown(KEY_ESCAPE) then
    ExitGame()
  end

  fire:update(dt)
end

function _render()
  ClearScene(bgColor)
  AmbientColor(bgColor)
  CameraPerspective(62, 0.1, 100)
  local mat = Matrix():rotate(getTime()/2,0,0) * viewMat
  mat:bind(VIEW)

  helpers.withEffect(lightFX, "Main", function (fx)
    fx:setLight("bonfire", light)
    roomScene:draw()
  end)

  Matrix():translate(-0.5,-0.3,0):scale(8):translate(fireSpot):translate(0,-2.2,0):rotate(-getTime()/2,0,0):bind(WORLD)
  fire:draw()
  Matrix():bind(WORLD)
end
