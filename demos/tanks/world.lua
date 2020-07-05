local terrain, terrainMaterial
world = {}

function initWorld()
    terrain = Model("terrain.fbx", false)
    terrainMaterial = Material()
    terrainMaterial:setDiffuse(0x815192)

    world = cols.newWorld()

    local meshNode = terrain:getRootNode():findNode("terrain")
    local mesh = meshNode:getMeshParts()[1][1]

    for i=0,(WORLD_TILES[1]-1),1 do
        for j=0,(WORLD_TILES[2]-1),1 do
        addTile(meshNode, mesh, Vector3(WORLD_SIZE*i,0,WORLD_SIZE*j))
        end
    end
end

function addTile(meshNode, mesh, pos)
    local terrainMesh = cols.newTriangleMeshFromPart(mesh, meshNode:getFinalTransform():translate(pos))
    terrainMesh.model = FaceGroup()

    for _, tr in pairs(terrainMesh.tris) do
        terrainMesh.model:addVertex(Vertex(tr[1]:x(),tr[1]:y(),tr[1]:z()))
        terrainMesh.model:addVertex(Vertex(tr[2]:x(),tr[2]:y(),tr[2]:z()))
        terrainMesh.model:addVertex(Vertex(tr[3]:x(),tr[3]:y(),tr[3]:z()))
    end
    terrainMesh.model:build()
    terrainMesh.model:calcNormals()

    world:addCollision(terrainMesh)
end

function drawWorld()
    BindTexture(0, terrainMaterial)
    for _, w in pairs(world.shapes) do
        w.model:draw()
    end
    BindTexture(0)
end