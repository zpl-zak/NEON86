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

local function drawEffect(fx, tech, drawfn)
  local numPasses = fx:start(tech)
  for i=1,numPasses do
    fx:beginPass(i)
    drawfn(fx)
    fx:endPass()
  end
  fx:finish()
end

local withEffect = drawEffect

local function spairs(t, order)
  -- collect the keys
  local keys = {}
  for k in pairs(t) do keys[#keys+1] = k end

  -- if order function given, sort by it by passing the table and keys a, b,
  -- otherwise just sort the keys
  if order then
      table.sort(keys, function(a,b) return order(t[a], t[b]) end)
  else
      table.sort(keys)
  end

  -- return the iterator function
  local i = 0
  return function()
      i = i + 1
      if keys[i] then
          return keys[i], t[keys[i]]
      end
  end
end

local function dump(o)
  if type(o) == 'table' then
     local s = '{ '
     for k,v in pairs(o) do
        if type(k) ~= 'number' then k = '"'..k..'"' end
        s = s .. '['..k..'] = ' .. dump(v) .. ','
     end
     return s .. '} '
  else
     return tostring(o)
  end
end

local function withTexture(tex, fn)
  BindTexture(0, tex)
  fn()
  BindTexture(0)
end

local function merge(a, b)
  if type(a) == 'table' and type(b) == 'table' then
      for k,v in pairs(b) do if type(v)=='table' and type(a[k] or false)=='table' then merge(a[k],v) else a[k]=v end end
  end
  return a
end

local helpers = {
  lerp = lerp,
  clamp = clamp,
  cap3 = cap3,
  spairs = spairs,
  dump = dump,
  merge = merge,
  drawEffect = drawEffect,
  withTexture = withTexture,
  withEffect = withEffect,
}

helpers.global = function()
  _G = merge(_G, helpers)
end

return helpers
