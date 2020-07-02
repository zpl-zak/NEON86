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
  return v:normalize() * m
end

return {
  lerp = lerp,
  clamp = clamp,
  cap3 = cap3
}