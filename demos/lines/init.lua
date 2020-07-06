local net = require "linesnetworking"

time = 0
local testFont
player = {}
testAI = {}
light = {}

WORLD_SIZE = 1000.0
WORLD_TILES = {5,5}

hh = require "helpers"
cols = require "collisions"

world = nil

dofile("trail.lua")
dofile("tank.lua")
dofile("world.lua")
dofile("player.lua")

local testSnd

function _init()
  RegisterFontFile("slkscr.ttf")
  testFont = Font("Silkscreen", 36, 1, false)
  testSnd = Sound("test.wav")
  testSnd:setVolume(65)
  testSnd:loop(true)
  testSnd:play()
  
  initWorld()
  initTankModel()
  setupTrail()

  math.random()
  math.random()
  math.random()
  math.random()
  addTank("local")
  -- testAI = addTank()
  
  setupPlayer()
  
  light = Light()
  light:setDirection(Vector(-1,-1,1))
  -- light:setSpecular(0xffffff)
  light:setSpecular(0)
  light:setDiffuse(0xffcc99)
  light:setType(LIGHTKIND_DIRECTIONAL)
  light:enable(true, 0)
end

function _net_tankupdate(entity_id, x, y, z, r)
  if tanks[entity_id] == nil then
    addTank(entity_id)
  end

  local tank = tanks[entity_id]

  tank.pos = Vector3(x,y,z)
  tank.heading = r

  -- LogString("_net_tankupdate: " .. entity_id .. " pos: " .. x .. " " .. y .. " " .. z)
end

function _update(dt)
  net.update()

  if GetKeyDown(KEY_ESCAPE) then
      ExitGame()
  end

  if GetKeyDown("R") then
      RestartGame()
  end

  if GetKeyDown("1") then
    net.server_start()
    net.connect("localhost")
  end

  if GetKeyDown("2") then
    net.connect("inlife.no-ip.org")
  end

  if IsFocused() then
    ShowCursor(false)
    SetCursorMode(CURSORMODE_CENTERED)
  else
    ShowCursor(true)
    SetCursorMode(CURSORMODE_DEFAULT)
  end

  updateTanks(dt)
  -- updateTestAI()
  player:update(dt, net)

  time = time + dt
end


function _render()
  EnableLighting(true)
  ClearScene(15,0,15)
  AmbientColor(0x773377)
  CameraPerspective(90, 1.0, 5000)
  Matrix():bind(WORLD)
  player.cam:bind(VIEW)
  SetFog(VectorRGBA(0,0,25,255), FOGKIND_LINEAR, 600, 1300)
  light:setDirection(Vector(-0.6,-1,-0.7))
  drawWorld()
  drawTanks()
end

function _render2d()
  testFont:drawText(0xFFFFFFFF, [[
WASD - move
shift - brake
  ]], 15, 30)
end

function updateTestAI()
  testAI.movedir = (player.tank.pos - testAI.pos):normalize()*0.001
end
