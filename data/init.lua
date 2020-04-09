spinner = 0.0
cube = nil
rot = nil
lookAt = nil
whiteTex = nil

function _init()
	CameraPerspective(45)
	-- CameraOrthographic(5,5)
	
	cube = {
		verts = {
			-- x, y, z, u, v, color
			Vertex( -1.0, 1.0, -1.0, 0, 0, Color(0, 0, 255)),
			Vertex( 1.0, 1.0, -1.0, -1.0, 0.0, Color(0, 0, 255)),
			Vertex( -1.0, -1.0, -1.0, 0.0, -1.0, Color(0, 0, 255)),
			Vertex( 1.0, -1.0, -1.0, -1.0, -1.0, Color(0, 0, 255)),
		},
		
		inds = {
			0, 1, 2,
			2, 1, 3,
		},
	}
	
	lookAt = Matrix():lookAt(
		Vector3(- 15, 5, 15),
		Vector3(),
		Vector3(0,1,0)
	)
	
	whiteTex = Texture("mafiahub.bmp")
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
	
	for i=0, 10, 1
	do
		for j=0, 10, 1
		do
			w = (rot * Matrix():translate(5,4*(i%2)-2,j*(-4)))
			cubeDraw(cube, w)
		end
	end
end

function euler(x,y,z)
	return Matrix():rotate(0,0,z) * Matrix():rotate(0,y,0) * Matrix():rotate(x,0,0)
end

function cubeDraw(cube, world)
	BindTexture(0, whiteTex)
	m = (Matrix() * world):bind(WORLD)
	DrawIndexedTriangle(cube.inds, cube.verts)
	
	m = (Matrix():rotate(0,90,0) * world):bind(WORLD)
	DrawIndexedTriangle(cube.inds, cube.verts)
	
	m = (Matrix():rotate(0,-90,0) * world):bind(WORLD)
	DrawIndexedTriangle(cube.inds, cube.verts)
	
	m = (Matrix():rotate(0,180,0) * world):bind(WORLD)
	DrawIndexedTriangle(cube.inds, cube.verts)
	
	m = (Matrix():rotate(90,0,0) * world):bind(WORLD)
	DrawIndexedTriangle(cube.inds, cube.verts)
	
	m = (Matrix():rotate(-90,0,0) * world):bind(WORLD)
	DrawIndexedTriangle(cube.inds, cube.verts)
	BindTexture(0)
end