-- Load model w/o materials
model = Model("assets/cube.fbx", false)

-- Assign albedo texture via Lua
-- 1. Find our mesh
cubeMesh = model:findMesh("Cube")

-- 2. Retrieve the first facegroup
faceGroup = cubeMesh:getFGroups()[1]

-- 3. Create a basic material for cube
cubeMaterial = Material()

-- 3.1. Load diffuse texture for our material
cubeMaterial:loadFile("assets/cube_albedo.png", 1)
cubeMaterial:loadFile("assets/cube_albedo.png", 2)

-- 4. Assign our material
faceGroup:setMaterial(0, cubeMaterial)

-- Create effect for our rendering pipeline
mainShader = Effect("main.fx")

-- Create our render target for post-processing effect
mainRT = RenderTarget()

-- Construct the camera view
viewMat = Matrix():lookAt(
    Vector(2,3,-5),
    Vector(0,0,0),
    Vector(0,1,0)
)

-- Track global time to simulate movement
time = 0

function _update(dt)
    if GetKeyDown(KEY_ESCAPE) then
        ExitGame()
    end

    time = time + dt
end

function _render()
    mainRT:bind()
    ClearScene(20,20,20)
    CameraPerspective(62)
    viewMat:bind(VIEW)
    
    -- Initialize the shader and load the Main technique
    mainShader:start("Main")

    -- Main technique has a single pass, use it
    mainShader:beginPass(1)

    -- Set up global shader variables and commit changes to the GPU
    mainShader:setVector4("ambience", Vector(1,0.05,0.05,1))
    mainShader:setFloat("time", time)
    mainShader:commit()

    -- Draw the scene
    drawScene()

    -- Finalize the pass
    mainShader:endPass()
    mainShader:finish()

    blitScreen()
end

function drawScene()

    -- Draw model with a specific transformation matrix
    model:draw(Matrix():rotate(time))
end

function blitScreen()
    ClearTarget()
    ClearScene(0,0,0)
 
    -- Initialize the shader and load the RTT technique
    mainShader:start("RTT")

    -- RTT technique has a Copy pass, use it
    mainShader:beginPass(1)

    -- Set up global shader variables and commit changes to the GPU
    mainShader:setTexture("sceneTex", mainRT)
    mainShader:commit()

    -- Draw the scene
    res = GetResolution()
    DrawQuad(0, res[1], 0, res[2], 0xFFFFFF)

    -- Finalize the pass and present changes to the screen
    mainShader:endPass()
    mainShader:finish()
end
