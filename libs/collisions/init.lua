local _ = {}

-- Sphere API

local Sphere = {}
Sphere.__index = Sphere

function Sphere.clone(self)
  return _.newSphere(self.pos, self.radius)
end

function Sphere.testSphere(self, pos, radius, move, fn)
  pos = pos + move
  local d = (self.pos - pos):mag()
  local r = (self.radius + radius)

  if d <= r then
    return {fn((self.pos - pos):normalize(), (r-d))}
  end
  if d == 0 then
    return {fn(Vector3(1,0,0), self.radius)}
  end
  return {}
end

function Sphere.testPoint(self, pos, move, fn)
  return self:testSphere(pos, 0, move, fn)
end

function Sphere.testMesh(self, mesh, move, fn)
  return mesh:testSphere(self.pos, self.radius, move:neg(), fn)
end

function Sphere.testBox(self, bounds, move, fn)
  return bounds:testSphere(self.pos, self.radius, move:neg(), fn)
end

-- TriangleMesh API

local TriangleMesh = {}
TriangleMesh.__index = TriangleMesh

function TriangleMesh.clone(self)
  return _.newTriangleMesh(self.tris, self.mat)
end

function TriangleMesh.testSphere(self, pos, radius, move, fn)
  local aabbContacts = self.bounds:testSphere(pos, radius, move, function ()
    return true
  end)

  if #aabbContacts < 1 then
    return
  end

  pos = pos + move
  local contacts = {}

  for _, tr in pairs(self.tris) do
    local v1 = tr[1]
    local v2 = tr[2]
    local v3 = tr[3]

    local u = v2 - v1
    local v = v3 - v1
    local n = u:cross(v)
    local w = pos - v1

    local n2 = squared(n)
    local a = (u:cross(w) * n) / n2
    local b = (w:cross(v) * n) / n2
    local c = 1 - a - b

    if (0 <= c) and (c <= 1) and
      (0 <= b) and (b <= 1) and
      (0 <= a) and (a <= 1) then
        local pp = (v1 * c) + (v2 * b) + (v3 * a)

        local d = (pp - pos):mag()
        if d <= radius then
          local pd = d - radius
          table.insert(contacts, {fn(u:cross(v):normalize(), pd, tr)})
        end
    end
  end

  return contacts
end

-- Box API

local Box = {}
Box.__index = Box

function Box.diameter(self)
  return (self.max - self.min):mag()
end

function Box.clone(self)
  return _.newBox({self.min, self.max, self.mat})
end

function Box.testSphere(self, pos, radius, move, fn)
  if move == nil then
    move = Vector()
  end
  pos = pos + move
  local sqDist = 0.0
  local p = pos:get()
  local min = self.min:get()
  local max = self.max:get()

  for i=1,3 do
    local v = p[i]
    if v < min[i] then
      sqDist = sqDist + squared(min[i] - v)
    end
    if v > max[i] then
      sqDist = sqDist + squared(v - max[i])
    end
  end

  local delta = sqDist - squared(radius)

  if delta <= 0 then
    local cp = (self.max - self.min) / 2
    local n = (cp - pos)
    return {fn(n:normalize(), (radius - n:mag()))}
  else
    return {}
  end
end

function Box.testPoint(self, pos, move, fn)
  return self:testSphere(pos, 0.0, move, fn)
end

function Box.testBox(self, box, move, fn)
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

  if ok then
    return {fn(Vector(), 0, BmaxV - self.min)}
  else
    return {}
  end
end

-- World API

local World = {}
World.__index = World

function World.addCollision(self, shape)
  table.insert(self.shapes, shape)
  return #self.shapes
end

function World.delCollision(self, idx)
  table.remove(self.shapes, idx)
end

function World.forEach(self, fn)
  for idx, shape in pairs(self.shapes) do
    fn(shape, idx)
  end
end

-- Public API

function _.newTriangleMesh(tris, mat)
  if mat == nil then
    mat = Matrix()
  end
  local self = setmetatable({}, TriangleMesh)
  self.tris = transformTriangles(tris, mat)
  self.bounds = _.newBox(calculateTrisMinMax(self.tris))
  self.mat = mat
  return self
end

function _.newTriangleMeshFromVertexData(data, mat)
  local tris = convertVertexDataToTris(data)
  return _.newTriangleMesh(tris, mat)
end

function _.newTriangleMeshFromPart(part, mat)
  return _.newTriangleMeshFromVertexData({part:getVertices(), part:getIndices()}, mat)
end

function _.newBox(data)
  local self = setmetatable({}, Box)
  self.min = data[1]
  self.max = data[2]
  self.dims = self.max - self.min
  return self
end

function _.newBoxFromVertexData(data, mat)
  local tris = convertVertexDataToTris(data)
  local bounds = calculateTrisMinMax(transformTriangles(tris, mat))
  return _.newBox(bounds)
end

function _.newBoxFromPart(part, mat)
  return _.newBoxFromVertexData({part:getVertices(), part:getIndices()}, mat)
end

function _.newSphere(pos, radius)
  local self = setmetatable({}, Sphere)
  self.pos = pos
  self.radius = radius
  return self
end

function _.newWorld()
  local self = setmetatable({}, World)
  self.shapes = {}
  return self
end

-- Response API

function _.slide(dir, norm)
  return _.bounce(dir, norm, 1.0)
end

function _.bounce(dir, norm, factor)
  if factor < 1.0 then
    factor = 1.0
  end

  return dir - (norm * (dir * norm))*factor
end

-- Helpers

function squared(a)
  return a*a
end

function calculateTrisMinMax(tris)
  local min = Vector3(math.maxinteger, math.maxinteger, math.maxinteger)
  local max = Vector3(math.mininteger, math.mininteger, math.mininteger)

  for _, tr in pairs(tris) do
    for _, v in pairs(tr) do
      for i=1,3 do
        local c = v:get()[i]
        if c < min:get()[i] then
          min = min:m(i, c)
        end
        if c > max:get()[i] then
          max = max:m(i, c)
        end
      end
    end
  end
  return {min, max}
end

function transformTriangles(tris, mat)
  local newTris = {}
  for idx, tr in pairs(tris) do
    local v1 = tr[1] * mat
    local v2 = tr[2] * mat
    local v3 = tr[3] * mat
    table.insert(newTris, {v1,v2,v3})
  end
  return newTris
end

function convertVertexDataToTris(data)
  local verts = data[1]
  local inds = data[2]
  local tris = {}
  local xyz = {}
  
  for _, vert in pairs(verts) do
    local vertData = vert:get()
    table.insert(xyz, Vector3(vertData[1], vertData[2], vertData[3]))
  end
  
  if inds ~= nil and #inds > 0 then
    for i=1,#inds,3 do
      local v1 = xyz[inds[i+0]+1]
      local v2 = xyz[inds[i+1]+1]
      local v3 = xyz[inds[i+2]+1]
      table.insert(tris, {v1,v2,v3})
    end
  else
    for i=1,#xyz,3 do
      local v1 = xyz[i+0]
      local v2 = xyz[i+1]
      local v3 = xyz[i+2]
      table.insert(tris, {v1,v2,v3})
    end
  end

  return tris
end

function findClosestPointToCenter(pos, v1, v2, v3)
  local cp = v1
  local d1 = (pos-v1):magSq()
  local d2 = (pos-v2):magSq()
  local d3 = (pos-v3):magSq()
  local sd = d1
  if d2 < sd then
    cp = v2
    sd = d2
  end
  if d3 < sd then
    cp = v3
    sd = d3
  end
  return cp
end

function distSq(v)
  return math.abs(squared(v:x()) + squared(v:y()) + squared(v:z()))
end

function area(a, b)
  return math.abs((a:x() * (b:y() - b:z()) + a:y() * (b:z() - b:x()) + a:z() * (b:x() - b:y())) / 2.0)
end

return _
