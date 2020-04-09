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