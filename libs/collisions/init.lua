local _ = {}

function squared(a)
    return a*a
end

-- Box API

local Box = {}

Box.__index = Box

function Box.diameter(self)
    return (self.max - self.min):mag()
end

function Box.clone(self)
    return _.newBox({self.min, self.max})
end

function Box.withDelta(self, delta)
    return _.newBox({self.min+delta, self.max+delta, self.mat, delta})
end

function Box.withMatrix(self, mat)
    return _.newBox({self.min*mat, self.max*mat, mat})
end

function Box.intersectsSphere(self, pos, radius, move)
    if move == nil then
        move = Vector()
    end
    local sqDist = 0.0
    local p = (pos + move):get()
    local min = self.min:get()
    local max = self.max:get()

    for i=1,3 do
        local v = p[i]
        if v < min[i] then
            sqDist += squared(min[i] - v)
        end
        if v > max[i] then
            sqDist += squared(v - max[i])
        end
    end

    local delta = sqDist - squared(radius)

    if delta <= 0 then
        return true, move:neg()
    else
        return false, move
    end
end

function Box.intersectsPoint(self, pos, move)
    return self:intersectsSphere(pos, 0.0, move)
end

function Box.intersects(self, box, move)
    if move == nil then
        move = Vector()
    end
    local Amin = self.min:get()
    local Amax = self.max:get()
    local Bmin = (box.min + move):get()
    local BmaxV = (box.max + move)
    local Bmax = BmaxV:get()

    ok = (Amin[1] <= Bmax[1] and Amax[1] >= Bmin[1]) and
         (Amin[2] <= Bmax[2] and Amax[2] >= Bmin[2]) and
         (Amin[3] <= Bmax[3] and Amax[3] >= Bmin[3])

    if not ok then
        move = move:neg()
    end

    return ok, move, (BmaxV - self.min)
end

-- Public API

function _.newBox(dims)
    local self = setmetatable({}, Box)
    self.min = dims[1]
    self.max = dims[2]
    self.mat = Matrix()
    self.dims = self.max - self.min

    if dims[3] ~= nil then
        self.mat = dims[3] * self.mat
    end

    if dims[4] ~= nil then
        self.mat = self.mat:translate(dims[4])
    end
    return self
end

return _
