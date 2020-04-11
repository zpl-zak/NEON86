spinner = 0.0
quad = nil
cube = nil
rot = nil
lookAt = Matrix():lookAt(
	Vector3(10,15,-20),
	Vector3(10,0,0),
	Vector3(0,1,0)
)
whiteTex = Texture("mafiahub.bmp")

dofile("another.lua")

function _init()
	CameraPerspective(45)
	-- CameraOrthographic(5,5)
	
	quad = Mesh()
	quad:addVertex(Vertex( -1.0, 1.0, -1.0, 0, 0, Color(255, 255, 255)))
	quad:addVertex(Vertex( 1.0, 1.0, -1.0, -1.0, 0.0, Color(255, 255, 255)))
	quad:addVertex(Vertex( -1.0, -1.0, -1.0, 0.0, -1.0, Color(255, 255, 255)))
	quad:addVertex(Vertex( 1.0, -1.0, -1.0, -1.0, -1.0, Color(255, 0, 0)))
	
	quad:addTriangle(0,1,2)
	quad:addTriangle(2,1,3)
	
	quad:setTexture(0, whiteTex)
	
	quad:build()

	cube = MeshGroup()
	cube:addMesh(quad, Matrix())
	cube:addMesh(quad, Matrix():rotate(0,math.rad(90),0))
	cube:addMesh(quad, Matrix():rotate(0,math.rad(-90),0))
	cube:addMesh(quad, Matrix():rotate(0,math.rad(180),0))
	cube:addMesh(quad, Matrix():rotate(math.rad(90),0,0))
	cube:addMesh(quad, Matrix():rotate(math.rad(-90),0,0))
end

function _destroy()

end

function _update(dt)
	spinner = spinner + 1.5*dt
	rot = euler(spinner, math.sin(spinner), 0)
end

function _render()
	ClearScene(0, 40, 100)
	
	lookAt:bind(VIEW)
	
	for i=0, 55, 1
	do
		for j=0, 55, 1
		do
			w = (rot * Matrix():translate(i*2, 0, j*2))
			cube:draw(w)
		end
	end
end
