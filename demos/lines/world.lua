local terrain, terrainMaterial, backdropModel, backdropMaterial
world = {}
local terrainShader

function initWorld()
    terrain = Model("terrain.fbx")
    backdropModel = Model("backdrop.fbx")
    terrainMaterial = Material("tile.png")
    terrainMaterial:setDiffuse(0x815192)
    terrainMaterial:setPower(120)

    backdropMaterial = Material("bounds.png")
    backdropMaterial:setEmission(0xD546A6)
    backdropMaterial:setDiffuse(0xD546A6)
    backdropMaterial:alphaIsTransparency(true)

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
    terrainShader:endPass()
    terrainShader:done()
    
    BindTexture(0, backdropMaterial)
    backdropModel:draw(Matrix():scale(WORLD_TILES[1], WORLD_TILES[1], WORLD_TILES[1]))
    BindTexture(0)
end
