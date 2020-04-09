spinner = 0.0
quad = nil
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
	
	quad = MeshBuilder()
	quad:addVertex(Vertex( -1.0, 1.0, -1.0, 0, 0, Color(0, 0, 255)))
	quad:addVertex(Vertex( 1.0, 1.0, -1.0, -1.0, 0.0, Color(0, 0, 255)))
	quad:addVertex(Vertex( -1.0, -1.0, -1.0, 0.0, -1.0, Color(0, 0, 255)))
	quad:addVertex(Vertex( 1.0, -1.0, -1.0, -1.0, -1.0, Color(0, 0, 255)))
	
	quad:addTriangle(0,1,2)
	quad:addTriangle(2,1,3)
	
	quad:setTexture(0, whiteTex)
	
	quad:build()
end

function _destroy()

end

function _update(dt)
	spinner = spinner + 1.5*dt
	rot = euler(spinner*(180.0/math.pi), math.sin(spinner)*(180.0/math.pi), 0)
end

function _render()
	ClearScene(0, 40, 100)
	
	lookAt:bind(VIEW)
	
	for i=0, 25, 1
	do
		for j=0, 25, 1
		do
			w = (rot * Matrix():translate(i*2, 0, j*2))
			cubeDraw(quad, w)
		end
	end
end
