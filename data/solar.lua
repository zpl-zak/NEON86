bodyModel = Model("body.fbx", false)
rotSpeed = 0.012

bodymats={
	sun={
        diffuse=Material("sun_albedo.jpg")
    },
	earth={
        diffuse=Material("earth_albedo.jpg"),
        spec=Material("earth_spec.jpg")
    },
	moon={
        diffuse=Material("moon_albedo.jpg"),
        normal=Material("moon_normal.jpg")
    }
}

function bindBodyMaterial(mat)
    local mesh = bodyModel:getMeshes()[1]
    local fg = mesh:getFGroups()[1]
    local bmat = fg:getMaterial()

    diffuse = nil
    spec = nil
    normal = nil

    if mat.diffuse ~= nil then
        diffuse = mat.diffuse:getHandle(1)
    end

    if mat.spec ~= nil then
        spec = mat.spec:getHandle(1)
    end
    
    if mat.normal ~= nil then
        normal = mat.normal:getHandle(1)
    end
    
    bmat:setHandle(TEXTURESLOT_ALBEDO, diffuse)
    bmat:setHandle(TEXTURESLOT_SPECULAR, spec)
    bmat:setHandle(TEXTURESLOT_NORMAL, normal)
end

function addBody(parent, model, offset, orbitSpeed, rotSpeed)
	local b={
        kind="natural",
		parent=parent,
		model=model,
		offset=offset,
		orbitSpeed=orbitSpeed,
		rotSpeed=rotSpeed,
        isSun=false,
        scale=1,
		bodies={}
	}

	if parent ~= nil then
		table.insert(parent.bodies, b)
	end

	return b
end

function generateSystem(pos, planetCount, maxMoonCount, moonGenChance)
	local s={
        bodies={},
        pos=pos
	}

    local sun = addBody(nil, bodymats.sun, Vector(), Vector(1/8, 0, 0), Vector(1/12,0,0))
    sun.isSun = true
    sun.scale = 4

    s.sun = sun

    for i=1,planetCount do
        local pos = Vector(
            math.random(48.0, 196.0)*math.random(-1,1),
            math.random(0.0, 4.0)*math.random(-1,1),
            math.random(48.0, 196.0)*math.random(-1,1))

        local orbit = Vector(1/math.random(29,32),0,0)
        local rot = Vector(math.random(1.0,4),0,0)
        local b = addBody(sun, bodymats.earth, pos, orbit, rot)

        for j=1,maxMoonCount do
            if math.random(0.0, 100.0) < moonGenChance then     
                local pos = Vector(
                    math.random(12.0, 23.0)*math.random(-1,1),
                    math.random(0.0, 4.0)*math.random(-1,1),
                    math.random(12.0, 23.0)*math.random(-1,1))

                local orbit = Vector(1/math.random(30,32),0,0)
                local rot = Vector(
                                math.random(0.0,32),
                                math.random(0.0,32),
                                math.random(0.0,32))
                local m = addBody(b, bodymats.moon, pos, orbit, rot)
                m.scale = 0.5
            end
        end
    end

	return s
end

function getTransform(b)
    if b == nil then
        return Matrix()
    end

    return Matrix()
        :scale(scale*b.scale)
        :rotate(b.rotSpeed*time*rotSpeed)
        :translate(b.offset*scale)
        :rotate(b.orbitSpeed*time*rotSpeed) * getTransform(b.parent)
end

function drawBody(b)
    local t = getTransform(b)
    
    spaceFX:setBool("isSun", b.isSun)
    bindBodyMaterial(b.model)
    bodyModel:draw(t)

    for i=1,#b.bodies do
        drawBody(b.bodies[i])
    end
end

function drawSystem(s)
    drawBody(s.sun)
end
