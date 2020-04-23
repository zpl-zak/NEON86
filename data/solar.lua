models={
	sun=Model("sun.fbx"),
	earth=Model("earth.fbx"),
	moon=Model("moon.fbx")
}

function addBody(parent, model, offset, orbitSpeed, rotSpeed)
	local b={
		parent=parent,
		model=model,
		offset=offset,
		orbitSpeed=orbitSpeed,
		rotSpeed=rotSpeed,
		isSun=false,
		bodies={}
	}

	if parent ~= nil then
		table.insert(parent.bodies, b)
	end

	return b
end

function generateSystem(pos, planetCount, maxMoonCount, moonGenFreq)
	local s={
        bodies={},
        pos=pos
	}

    local sun = addBody(s, models.sun, Vector(), Vector(1/8, 0, 0), Vector(1/12,0,0))
    sun.isSun = true

    for i=1,planetCount do
        local pos = Vector(
            math.random(12.0, 196.0)*math.random(-1,1),
            math.random(12.0, 196.0)*math.random(-1,1),
            math.random(12.0, 196.0)*math.random(-1,1))

        local orbit = Vector(1/math.random(2,32),0,0)
        local rot = Vector(1/math.random(2,32),0,0)
        local b = addBody(sun, models.earth, pos, orbit, rot)
    end

	return s
end

function drawBody(b, tpos)
    
    local t = Matrix()
        :scale(scale)
        :rotate(b.rotSpeed*time)
        :translate(b.offset*scale + tpos)
        :rotate(b.orbitSpeed*time)
    
    spaceFX:setBool("isSun", b.isSun)
    b.model:draw(t)

    for i=1,#b.bodies do
        drawBody(b.bodies[i], tpos+b.offset)
    end
end

function drawSystem(s)
    for i=1,#s.bodies do
        drawBody(s.bodies[i], s.pos)
    end
end
