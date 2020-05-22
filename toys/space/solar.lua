bodyModel = Model("body.fbx", true, true)
rotSpeed = 0.1

bodymats={
	sun={
        {
            diffuse=Material("sun_albedo.jpg")
        }
    },
	earth={
        {
            diffuse=Material("earth_albedo.jpg"),
            spec=Material("earth_spec.jpg"),
            normal=Material("earth_normal.jpg")
        },
        {
            diffuse=Material("mars_albedo.jpg"),
            normal=Material("mars_normal.jpg")
        }
    },
	moon={
        {
            diffuse=Material("moon_albedo.jpg"),
            normal=Material("moon_normal.jpg")
        }
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
		model=model[math.random(1,#model)],
		offset=offset,
		orbitSpeed=orbitSpeed,
		rotSpeed=rotSpeed,
        initialRot=Vector3(
            math.random(),
            math.random(),
            math.random()
        ),
        isSun=false,
        scale=1,
        bodies={},
	}

	if parent ~= nil then
		table.insert(parent.bodies, b)
	end

	return b
end

function generateOffset(f)
    local x = math.random() - 0.5
    local y = (math.random() - 0.5)*0.02;
    local z = math.random() - 0.5

    local m = math.sqrt(x*x + y*y + z*z)
    x = x / m; y = y / m; z = z / m;

    local d = (math.random() + 5) * 1
    return Vector(x*d, y*d, z*d)
end

function generateSystem(pos, planetCount, maxMoonCount, moonGenChance)
	local s={
        bodies={},
        pos=pos
	}

    local sun = addBody(nil, bodymats.sun, Vector(), Vector(), Vector(rotSpeed,0,0))
    sun.isSun = true
    sun.scale = 100

    s.sun = sun

    for i=1,planetCount do
        local pos = generateOffset(1)

        local orbit = Vector(1/math.random(29,32),0,0)
        local rot = Vector(1/math.random(1.0,2),0,0)
        local b = addBody(sun, bodymats.earth, pos, orbit, rot)
        b.scale = 0.1 + math.random()*0.15

        for j=1,maxMoonCount do
            if math.random(0.0, 100.0) < moonGenChance then     
                local pos = generateOffset(1)

                local orbit = Vector(math.random(0,30)/10.0,0,0)
                local rot = Vector(
                                math.random(0.0,30)/10.0,0,0)
                local m = addBody(b, bodymats.moon, pos, orbit, rot)
                m.scale = 0.15 + math.random()*0.3
            end
        end
    end

	return s
end

function getTransform(b, p)
    if b == nil then
        return Matrix()
    end

    selfRot = b.initialRot + b.rotSpeed*time

    if b ~= p then
        selfRot = Vector()
    end

    return Matrix()
        :scale(scale*b.scale)
        --:translate(0,0,0)

        -- self orientation
        :rotate(selfRot:x(),0,0)
        :rotate(0, selfRot:y(), 0)
        :rotate(selfRot:z(),0,0)

        -- position
        :translate(b.offset*scale)

        -- orbit
        :rotate(b.orbitSpeed:x()*time*rotSpeed,0,0)
        :rotate(0,b.orbitSpeed:y()*time*rotSpeed,0)
        :rotate(0,0,b.orbitSpeed:z()*time*rotSpeed) * getTransform(b.parent, p)
end

function drawBody(b)
    local t = getTransform(b, b)
    
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
