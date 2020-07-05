time = 0
local testFont
player = {}
testAI = {}
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
  testAI = addTank()
  
  setupPlayer()
  
  light = Light()
  light:setDirection(Vector(-0.6,-1,-0.7))
  -- light:setSpecular(0xffffff)
  light:setSpecular(0)
  light:setDiffuse(0xffcc99)
  light:setType(LIGHTKIND_DIRECTIONAL)
  light:enable(true, 0)
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
  updateTestAI()
  player:update(dt)
  time = time + dt
end

function _render()
  EnableLighting(true)
  ToggleWireframe(true)
  ClearScene(15,0,15)
  AmbientColor(0x440844)
  CameraPerspective(90, 0.1, 5000)
  Matrix():bind(WORLD)
  player.cam:bind(VIEW)
  -- SetFog(VectorRGBA(0,0,255,255), FOGKIND_LINEAR, 600, 1300)
  SetFog(VectorRGBA(0,0,25,255), FOGKIND_LINEAR, 600, 1300)
  light:setDirection(Vector(-0.6,-1,-0.7))
  
  -- ClearFog()
  drawTanks()
  ToggleWireframe(false)
  drawWorld()
end

function _render2d()
  testFont:drawText(0xFFFFFFFF, [[
WASD - move
shift - brake
  ]], 15, 30)
end

function updateTestAI()
  testAI.movedir = (player.tank.pos - testAI.pos):normalize()*0.1
end