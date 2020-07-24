function lerp(a,b,t)
  return a + (b-a)*t
end

function clamp(a,x,b)
  if x < a then
    return a
  end

  if x > b then
    return b
  end

  return x
end

function cap3(v, m)
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

function drawEffect(fx, tech, drawfn)
  fx:start(tech)
  fx:beginPass(1)
  drawfn(fx)
  fx:endPass()
  fx:finish()
end

function spairs(t, order)
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

return {
  lerp = lerp,
  clamp = clamp,
  cap3 = cap3,
  drawEffect = drawEffect,
  spairs = spairs
}