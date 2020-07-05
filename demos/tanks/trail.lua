local trailMaterial
MAX_TRAILS = 150.0
TRAIL_TIME = 0.05

function setupTrail()
    trailMaterial = Material("trail.png")
    trailMaterial:setDiffuse(255,0,0)
    trailMaterial:setEmission(255,0,0)
    trailMaterial:setOpacity(1)
    trailMaterial:alphaIsTransparency(true)
end

function getTrailPos(t, trailNode)
    return t.pos + (trailNode:getFinalTransform():translate(0,t.hover:y(),0) * t.rot):row(4)
end

function handleTrails(t, trailNode)
    if t.trailTime < time then
        t.trailTime = time + TRAIL_TIME

        if #t.trails > MAX_TRAILS then
            table.remove(t.trails, 1)
        end
        table.insert(t.trails, getTrailPos(t, trailNode))
    end
end

function drawTrails(tank, height, trailNode)
    local trails = tank.trails
    for i=1,#trails,1 do
        local tr1 = trails[i]
        local tr2 = trails[i+1]
        
        if tr1 ~= nil then
            if #trails > MAX_TRAILS then
                trailMaterial:setOpacity(i/MAX_TRAILS)
            end
            if tr2 == nil then
                tr2 = getTrailPos(tank, trailNode)
                trailMaterial:setOpacity(1)
            end
            
            BindTexture(0, trailMaterial)
            Matrix():bind(WORLD)
            ToggleWireframe(false)
            CullMode(CULLKIND_NONE)
            DrawPolygon(
                Vertex(tr1:x()-height, tr1:y(), tr1:z(), 0, 0),
                Vertex(tr1:x()+height, tr1:y(), tr1:z(), 0, 1),
                Vertex(tr2:x()-height, tr2:y(), tr2:z(), 1, 0)
            )
            DrawPolygon(
                Vertex(tr1:x()+height, tr1:y(), tr1:z(), 0, 1),
                Vertex(tr2:x()-height, tr2:y(), tr2:z(), 1, 0),
                Vertex(tr2:x()+height, tr2:y(), tr2:z(), 1, 1)
            )
            ToggleWireframe(true)
        end
    end
    BindTexture(0)
end