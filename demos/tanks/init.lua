time = 0
local testFont
player = {}
local light

WORLD_SIZE = 1000.0
WORLD_TILES = {5,5}

hh = require "helpers"
cols = require "collisions"

world = nil

dofile("trail.lua")
dofile("tank.lua")
dofile("world.lua")
dofile("player.lua")

function _init()
  RegisterFontFile("slkscr.ttf")
  testFont = Font("Silkscreen", 36, 1, false)

  initWorld()
  initTankModel()
  setupTrail()

  math.random()
  math.random()
  math.random()
  math.random()
  addTank()

  setupPlayer()

  light = Light()
  light:setDirection(Vector(-1,-1,1))
  light:setDiffuse(255,255,255)
  light:setType(LIGHTKIND_DIRECTIONAL)
  light:enable(true, 0)

  SetFog(VectorRGBA(0,0,255,255), FOGKIND_LINEAR, 600, 1300)
end

function _update(dt)
  if GetKeyDown(KEY_ESCAPE) then
      ExitGame()
  end

  if IsFocused() then
    ShowCursor(false)
    SetCursorMode(CURSORMODE_CENTERED)
  else
    ShowCursor(true)
    SetCursorMode(CURSORMODE_DEFAULT)
  end

  updateTanks(dt)
  player:update(dt)
  time = time + dt
end

function _render()
  EnableLighting(true)
  ToggleWireframe(true)
  ClearScene(15,0,15)
  AmbientColor(0x440844)
  CameraPerspective(62, 0.1, 5000)
  Matrix():bind(WORLD)
  player.cam:bind(VIEW)
  drawWorld()
  drawTanks()
  ToggleWireframe(false)
end

function _render2d()
  testFont:drawText(0xFFFFFFFF, [[
WASD - move
shift - brake
  ]], 15, 30)
end
