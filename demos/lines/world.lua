local terrain, terrainMaterial
world = {}
local terrainShader

function initWorld()
    terrain = Model("terrain.fbx")
    terrainMaterial = Material("tile.png")
    terrainMaterial:setDiffuse(0x815192)
    terrainMaterial:setPower(120)

    world = cols.newWorld()

    local meshNode = terrain:getRootNode():findNode("terrain")
    local mesh = meshNode:getMeshParts()[1][1]

    for i=0,(WORLD_TILES[1]-1),1 do
        for j=0,(WORLD_TILES[2]-1),1 do
        addTile(meshNode, mesh, Vector3(WORLD_SIZE*i,0,WORLD_SIZE*j))
        end
    end

    terrainShader = Effect("terrain.fx")
end

function addTile(meshNode, mesh, pos)
    local terrainMesh = cols.newTriangleMeshFromPart(mesh, meshNode:getFinalTransform():translate(pos))
    terrainMesh.pos = Matrix():translate(pos)

    world:addCollision(terrainMesh)
end

function drawWorld()
    terrainShader:begin("Main")
    terrainShader:beginPass("Default")
    terrainShader:setFloat("time", time)
    terrainShader:setVector3("campos", player.pos:neg())
    terrainShader:setLight("sun", light)
    terrainShader:commit()
    BindTexture(0, terrainMaterial)
    for _, w in pairs(world.shapes) do
        terrain:draw(w.pos)
    end
    BindTexture(0)
    terrainShader:flush()
    terrainShader:done()
end
