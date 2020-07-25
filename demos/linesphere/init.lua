-- Minimal NEON86 game template

testFont = Font("Times New Roman", 36, 2000, false)
bgColor = Color(20,20,69)
toggleLighting = true
light = Light()
contact = Model("sphere.fbx", false)

cam = require "camera" (Vector(0,0,-5))

light:setDirection(Vector(-1,-1,1))
light:enable(true, 0)

lineMat = Material()
lineMat:setDiffuse(255,0,0)

viewMat = Matrix():lookAt(
      Vector(0,0,-5),
      Vector(),
      Vector(0,1,0)
)

tr1 = {-5,0,0}
tr2 = {5,0,0}

function solveParam(b, dis)
  t = (-b - math.sqrt(dis))
  if t > 0 then return t end

  t = (-b + math.sqrt(dis))
  if t > 0 then return t end

  return -1
end

function updateCols()
  A = Vector3(tr1[1], tr1[2], tr1[3])
  B = Vector3(tr2[1], tr2[2], tr2[3])
  C = cam.pos
  AB = (B-A)
  d = AB:normalize()
  r = 5

  oc = A-C

  a = d*d
  b = 2*(d*oc)
  c = oc:magSq() - r*r

  dis = b*b - 4*a*c

  if dis < 0 then
    lineMat:setDiffuse(255,0,0)
  else
    t = solveParam(b, dis) / (a*2)

    if t > 0 and (d*t):magSq() < AB:magSq() then
      lineMat:setDiffuse(0,255,0)
      contact:draw(Matrix():scale(0.1,0.1,0.1):translate(A + d*t))
    else
      lineMat:setDiffuse(255,0,0)
    end
  end
end

function _update(dt)
  if GetKeyDown(KEY_ESCAPE) then
    ExitGame()
  end

  if GetKeyDown("l") then
    toggleLighting = not toggleLighting
  end

  if GetMouse(MOUSE_RIGHT_BUTTON) then
    ShowCursor(false)
    SetCursorMode(CURSORMODE_CENTERED)
  else
    ShowCursor(true)
    SetCursorMode(CURSORMODE_DEFAULT)
  end

  cam:update(dt)
end

function _render()
  ClearScene(bgColor)
  AmbientColor(bgColor)
  CameraPerspective(62, 0.1, 100)
  EnableLighting(toggleLighting)
  cam.mat:bind(VIEW)
  updateCols()

  height = 5

  CullMode(CULLKIND_NONE)
  BindTexture(0, lineMat)
  DrawPolygon(
    Vertex(tr1[1], tr1[2]-height, tr1[3], 0, 0),
    Vertex(tr1[1], tr1[2]+height, tr1[3], 0, 1),
    Vertex(tr2[1], tr2[2]-height, tr2[3], 1, 0)
  )
  DrawPolygon(
    Vertex(tr2[1], tr2[2]+height, tr2[3], 1, 1),
    Vertex(tr2[1], tr2[2]-height, tr2[3], 1, 0),
    Vertex(tr1[1], tr1[2]+height, tr1[3], 0, 1)
  )
  BindTexture(0)
end

function _render2d()
  testFont:drawText(0xFFFFFFFF, [[
    Welcome to the NEON86 basic template!

    Press L to toggle lighting
  ]], 0, 30)
end
