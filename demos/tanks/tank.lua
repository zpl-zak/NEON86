tankModel = {}
tanks = {}
local tankMaterial

local tankBody, tankCanon, tankCanonMat, trailPosNode

function initTankModel()
    tankModel = Model("tank.fbx", false, false)
    local tankNode = tankModel:getRootNode()
    tankBody = tankNode:findNode("body")
    trailPosNode = tankNode:findNode("trailpos")
    tankMaterial = Material()
    tankMaterial:setAmbient(0xe6cfff)
end

function addTank(id)
    tanks[id] = {
      pos = Vector3(
        math.random(0,WORLD_SIZE*WORLD_TILES[1]),
        10,
        math.random(0,WORLD_SIZE*WORLD_TILES[2])
      ),
      hover = Vector3(),
      movedir = Vector3(),
      vel = Vector(),
      rot = Matrix(),
      trails = {},
      trailTime = 0,
      crot = 0,
      health = 100
    }
end

function updateTanks(dt)
    local t = tanks["local"]

    -- for _, t in pairs(tanks) do
      t.vel:y(t.vel:y() - 2*dt)
      world:forEach(function (shape)
        shape:testSphere(t.pos, 5, t.vel, function (norm)
          t.vel = cols.slide(t.vel+t.movedir*2, norm)
        end)
      end)
      local hoverFactor = 2
      t.vel = t.vel:lerp(t.vel+t.movedir, 1.0)
      t.hover = Vector3(0,math.sin(time*4) * (hoverFactor - math.min(t.vel:magSq(), hoverFactor) / hoverFactor),0)
      t.pos = t.pos + t.vel
      t.crotm = t.rot
      
      if t.pos:x() <= 0 then
        t.vel:x(-t.vel:x())
        t.pos:x(t.pos:x()+t.vel:x())
      end
      
      if t.pos:z() <= 0 then
        t.vel:z(-t.vel:z())
        t.pos:x(t.pos:x()+t.vel:x())
      end
      
      if t.pos:x() >= WORLD_SIZE*WORLD_TILES[1] then
        t.vel:x(-t.vel:x())
        t.pos:x(t.pos:x()+t.vel:x())
      end
      
      if t.pos:z() >= WORLD_SIZE*WORLD_TILES[2] then
        t.vel:z(-t.vel:z())
        t.pos:z(t.pos:z()+t.vel:z())
      end
  
      if t.pos:y() < -100 then
        t.pos:y(15)
        t.vel:y(1)
      end

      handleTrails(t, trailPosNode)
    -- end
end

function drawTanks()
    BindTexture(0, tankMaterial)
    for id, t in pairs(tanks) do
        LogString("draawing tank #"..id .. " pso: " .. vec2str(t.pos))
        Matrix():bind(WORLD)
        tankBody:draw(t.rot * Matrix():translate(t.pos+t.hover))
        drawTrails(t, 5, trailPosNode)
    end
    BindTexture(0)
end
