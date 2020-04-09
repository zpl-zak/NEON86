spinner = 0.0
cube = nil

function _init()
	CameraPerspective(45)
	-- CameraOrthographic(5,5)
	
	cube = Cube()
end

function _destroy()

end

function _update(dt)
	spinner = spinner + 1.5*dt
end

function _render()
	ClearScene(0, 40, 100)
	
	rot = euler(spinner, math.sin(spinner), 0)
	
	Matrix():lookAt(
		Vector3(0, 5, 20),
		Vector3(),
		Vector3(0,1,0)
	):bind(VIEW)
	
	for i=0, 10, 1
	do
		(rot * Matrix():translate(i%10,i%2-1,i*(-1))):bind(WORLD)
		drawObjectIndexed(cube)
	end
end

function drawObjectIndexed(obj)
	DrawIndexedTriangle(obj.inds, obj.verts)
end

function euler(x,y,z)
	rotX = Matrix()
	rotY = Matrix()
	rotZ = Matrix()
	
	rotX:rotate(x,0,0)
	rotY:rotate(0,y,0)
	rotZ:rotate(0,0,z)
	
	rot = Matrix()
	rot = rotZ * rotY * rotX
	
	return rot
end

function Cube()
	return {
		verts = {
			Vertex( -1.0, 1.0, -1.0, Color(0, 0, 255)),
			Vertex( 1.0, 1.0, -1.0, Color(0, 0, 255)),
			Vertex( -1.0, -1.0, -1.0, Color(0, 0, 255)),
			Vertex( 1.0, -1.0, -1.0, Color(0, 255, 255)),
			Vertex( -1.0, 1.0, 1.0, Color(0, 0, 255)),
			Vertex( 1.0, 1.0, 1.0, Color(255, 0, 0)),
			Vertex( -1.0, -1.0, 1.0, Color(0, 255, 0)),
			Vertex( 1.0, -1.0, 1.0, Color(0, 255, 255)),
		},
		
		inds = {
			0, 1, 2,    -- side 1
			2, 1, 3,
			4, 0, 6,    -- side 2
			6, 0, 2,
			7, 5, 6,    -- side 3
			6, 5, 4,
			3, 1, 7,    -- side 4
			7, 1, 5,
			4, 5, 0,    -- side 5
			0, 5, 1,
			3, 7, 2,    -- side 6
			2, 7, 6,
		}
	}
end