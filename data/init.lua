rotY = 0.0

function _init()
	CameraPerspective(45)
	-- CameraOrthographic(5,5)
end

function _destroy()

end

function _update(dt)
	rotY = rotY + 1.5*dt
end

function _render()
	ClearScene(0, 40, 100)
	
	matRotateY = Matrix()
	matRotateX = Matrix()
	
	matRotateY:rotate(0, rotY, 0)
	matRotateX:rotate(rotY, 0, 0)
	
	matRotate = matRotateY * matRotateX
	matRotate:bind(WORLD)
	
	eyePos = Vector3(0, 0, 10)
	atPos = Vector3()
	upPos = Vector3(0,1,0)
	
	matView = Matrix( )
	matView:lookAt(eyePos, atPos, upPos)
	matView:bind(VIEW)
	
	cube = Cube()
	
	DrawIndexedTriangle(cube.inds, cube.verts)
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