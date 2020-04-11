function euler(x,y,z)
	return Matrix():rotate(0,0,z) * Matrix():rotate(0,y,0) * Matrix():rotate(x,0,0)
end
