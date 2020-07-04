local function lerp(a,b,t)
  return a + (b-a)*t
end

local function clamp(a,x,b)
  if x < a then
    return a
  end

  if x > b then
    return b
  end

  return x
end

local function cap3(v, m)
  if math.abs(v:x()) > m then
    return v / v:x() * m
  end
  if math.abs(v:y()) > m then
    return v / v:y() * m
  end
  if math.abs(v:z()) > m then
    return v / v:z() * m
  end
  return v
end

return {
  lerp = lerp,
  clamp = clamp,
  cap3 = cap3
}