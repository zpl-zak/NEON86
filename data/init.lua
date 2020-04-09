spinner = 0.0
quad = nil
rot = nil
lookAt = nil
whiteTex = nil

function _init()
	CameraPerspective(45)
	-- CameraOrthographic(5,5)
	
	whiteTex = Texture("mafiahub.bmp")
	
	quad = MeshBuilder()
	quad:addVertex(Vertex( -1.0, 1.0, -1.0, 0, 0, Color(0, 0, 255)))
	quad:addVertex(Vertex( 1.0, 1.0, -1.0, -1.0, 0.0, Color(0, 0, 255)))
	quad:addVertex(Vertex( -1.0, -1.0, -1.0, 0.0, -1.0, Color(0, 0, 255)))
	quad:addVertex(Vertex( 1.0, -1.0, -1.0, -1.0, -1.0, Color(0, 0, 255)))
	
	quad:addTriangle(0,1,2)
	quad:addTriangle(2,1,3)
	
	quad:setTexture(0, whiteTex)
	
	quad:build()
	
	lookAt = Matrix():lookAt(
		Vector3(10,15,-20),
		Vector3(10,0,0),
		Vector3(0,1,0)
	)
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

function euler(x,y,z)
	return Matrix():rotate(0,0,z) * Matrix():rotate(0,y,0) * Matrix():rotate(x,0,0)
end

function cubeDraw(quad, world)
	m = (Matrix() * world):bind(WORLD)
	quad:draw()
	
	m = (Matrix():rotate(0,90,0) * world):bind(WORLD)
	quad:draw()
	
	m = (Matrix():rotate(0,-90,0) * world):bind(WORLD)
	quad:draw()
	
	m = (Matrix():rotate(0,180,0) * world):bind(WORLD)
	quad:draw()
	
	m = (Matrix():rotate(90,0,0) * world):bind(WORLD)
	quad:draw()
	
	m = (Matrix():rotate(-90,0,0) * world):bind(WORLD)
	quad:draw()
end