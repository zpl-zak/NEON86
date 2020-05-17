local root
local clonedRoot
local time = 0

function _init()
    root = Node()
    local model = Model("sphere.fbx"):getMeshes()[1]

    local child = Node()
    child:setTransform(Matrix():translate(0, 2, 0):rotate(3.14,0,0))
    child:addMesh(model)

    root:addNode(child)
    root:addMesh(model)

    clonedRoot = root:clone()
end

function _update()
    if GetKeyDown(KEY_ESCAPE) then
        ExitGame()
    end

    time = getTime()

    root:setTransform(Matrix():rotate(math.sin(time), 0, 0))
end

function _render()
    ClearScene(0,128,128)
    AmbientColor(16,16,16)

    Matrix():lookAt(
        Vector(0,0,-5),
        Vector(),
        Vector(0,1,0)
    ):bind(VIEW)
    CameraPerspective(62, 0.1, 100)

    root:draw(Matrix():translate(-2,-1,0))
    clonedRoot:draw(Matrix():translate(2,-1,0))
end