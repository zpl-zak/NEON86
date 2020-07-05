local net = require "sampleplugin"

time = 0
local testFont
player = {}

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

  addTank("local")

  setupPlayer()

  SetFog(VectorRGBA((0xe6/0xff)*0.80,(0xcf/0xff)*0.80,(0xff/0xff)*0.80), FOGKIND_EXP, 0.00112)
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

  if GetKeyDown("1") then
    net.server_start()
    net.connect("localhost")
  end

  if GetKeyDown("2") then
    net.connect("localhost")
  end

  if IsFocused() then
    ShowCursor(false)
    SetCursorMode(CURSORMODE_CENTERED)
  else
    ShowCursor(true)
    SetCursorMode(CURSORMODE_DEFAULT)
  end

  updateTanks(dt)
  player:update(dt, net)
  time = time + dt
end


function _render()
  EnableLighting(true)
  ToggleWireframe(true)
  ClearScene(5,0,5)
  AmbientColor(0xffffff)
  CameraPerspective(62, 0.1, 5000)
  player.cam:bind(VIEW)
  drawWorld()
  drawTanks()
  ToggleWireframe(false)
end

function _render2d()
  testFont:drawText(0xFFFFFFFF, "demo", 15, 30)
end
