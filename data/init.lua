-- Load model w/o materials
model = Model("assets/cube.fbx", false)

-- Assign albedo texture via Lua
-- 1. Find our mesh
cubeMesh = model:findMesh("Cube")

-- 2. Retrieve the first facegroup
faceGroup = cubeMesh:getFGroups()[1]

-- 3. Create a basic material for cube
cubeMaterial = Material()

-- 3.1. Load diffuse and specular texture for our material
cubeMaterial:loadFile("assets/cube_albedo.png", 1)
cubeMaterial:loadFile("assets/cube_albedo.png", 2)

-- 4. Assign our material
faceGroup:setMaterial(0, cubeMaterial)

-- Floor model
floor = Model("assets/floor.fbx")
floor:findMesh("Plane"):getFGroups()[1]:setMaterial(0, cubeMaterial)

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

function drawShadedScene()
    mainRT:bind()
    ClearScene(20,20,20)
    CameraPerspective(62, 2, 10)
    viewMat = Matrix()
        :rotate(time/4, 0, 0)
        :translate(0,0,5)
        :bind(VIEW)

    -- Initialize the shader and load the Main technique
    mainShader:start("Main")

    -- Main technique has a single pass, use it
    mainShader:beginPass("Default")

    -- Set up global shader variables and commit changes to the GPU
    mainShader:setVector4("ambience", Vector(1,0.05,0.05,1))
    mainShader:setTexture("shadowTex", shadowMap)
    mainShader:setFloat("time", time)
    mainShader:commit()

    -- Draw the scene
    drawScene()

    -- Finalize the pass
    mainShader:endPass()
    mainShader:finish()
    ClearTarget()
end

function _render()
    ClearScene(20,20,20)
    CameraPerspective(62, 2, 100)
    viewMat:bind(VIEW)
    drawShadedScene()

    -- blitScreen(mainRT)
    blitScreenFFP(mainRT)
    -- drawScene()
end

function drawScene()

    -- Draw model with a specific transformation matrix
    model:draw(Matrix():rotate(time))
    floor:draw(Matrix():translate(0,-2,0))
end

-- Blit rendered RenderTarget using a RTT shader
-- This allows us to apply various Post-FX effects
function blitScreen(rt)
    ClearTarget()
    ClearScene(0,0,0)
 
    -- Initialize the shader and load the RTT technique
    mainShader:start("RTT")

    -- RTT technique has a Copy pass, use it
    mainShader:beginPass("Copy")

    -- Set up global shader variables and commit changes to the GPU
    mainShader:setTexture("sceneTex", rt)
    mainShader:commit()

    -- Copy the RT via shader
    FillScreen()

    -- Finalize the pass and present changes to the screen
    mainShader:endPass()
    mainShader:finish()
end

-- Blit rendered RenderTarget using FFP
-- While easy to perform, it doesn't offer any significant advantage over shaders
function blitScreenFFP(rt)
    ClearTarget()
    ClearScene(0,0,0)
    BindTexture(0, rt)

    -- Copy the RT directly into backbuffer
    FillScreen(0xFFFFFFFF, true)
    BindTexture(0, 0)
end
