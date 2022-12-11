local mapModel, colsModel
local camera

local bgColor = VectorRGBA(42, 16, 23)

local lights = {}

shader = Effect("fx/main.fx")

require "helpers".global()

local Pipe = require "pipe"
local drawPipe = Pipe()

-- Create a sunlight
sun = Light()
sun:setType(LIGHTKIND_DIRECTIONAL)
sun:setDirection(Vector(-1, -1, 1))
sun:setDiffuse(Color(32, 46, 90))
sun:setSpecular(Color(32, 46, 90))
sun:setAttenuation(1, 1, 0)
sun:enable(true, 0)

-- Register local font and create font drawer
RegisterFontFile("slkscr.ttf")
testFont = Font("Silkscreen", 16, 700, false)

-- Initialize camera
local Camera = require "camera"
camera = Camera(Vector3(-39.431, 3.21591, 1.2078))
camera:fixAngle({ 0.60000003129245, 0.097500005085019 })
camera.grounded = false

function _init()
  mapModel = Model("map.fbx", true, false)

  idx = 1
  for a, lv in pairs(mapModel:getRootNode():findNode("lights"):getTargets()) do
    l = Light()
    l:setType(LIGHTKIND_POINT)
    l:setPosition(lv:row(4))
    l:setDiffuse(206, 79, 47)
    l:setSpecular(0.12, 0.12, 0.12, 1)
    l:setRange(25)
    l:setAttenuation(0, 0, 2)
    l:enable(true, idx)
    table.insert(lights, l)
    idx = idx
  end

  ShowCursor(false)
  SetCursorMode(CURSORMODE_CENTERED)
end

function _fixedUpdate(dt)
  if GetKeyDown(KEY_ESCAPE) then
    ExitGame()
  end

  if GetKeyDown(KEY_F2) then
    ShowCursor(not IsCursorVisible())
    SetCursorMode(1 - GetCursorMode())
  end

  if GetKeyDown(KEY_F3) then
    LogString("campos: " .. vec2str(camera.pos) .. "\n")
    LogString("camangle: " .. camera.angles[1] .. ", " .. camera.angles[2] .. "\n")
  end

  time = getTime()
  camera.grounded = false
end

function _update(dt)
  camera:update(dt)
end

function _render()
  ClearScene(bgColor:color())
  AmbientColor(bgColor:color())

  camera.mat:bind(VIEW)
  CameraPerspective(62, 0.1, 9000)
  drawPipe:push(mapModel)

  withEffect(shader, "Lit", function(fx)
    fx:setLight("sun", sun)
    fx:setVector3("campos", camera.pos)
    for i = 1, 32 do
      fx:setBool("lights[" .. (i - 1) .. "].IsEnabled", false)
    end
    for i, l in pairs(lights) do
      fx:setLight("lights[" .. (i - 1) .. "]", l)
    end
    fx:setInt("numLights", #lights)
    fx:commit()
    drawPipe:draw()
  end)
end

function _render2d()
  testFont:drawText(0xFFFFFFFF, [[

Tristram map viewer

Map made by Cukier for Mafia: The City of Lost Heaven.

  ]], 15, 30, 1200, 800)

  testFont:drawText(Color(255, 64, 0), "Move with WASD", 15, 600)
end
