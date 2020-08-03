-- Minimal NEON86 game template

testFont = Font("Times New Roman", 36, 2000, false)
sphere = Model("sphere.fbx")
bgColor = Color(20,20,69)
toggleLighting = true
light = Light()
music = Music("music.ogg")
music:play()
music:setVolume(100)

light:setDirection(Vector(-1,-1,1))
light:enable(true, 0)

viewMat = Matrix():lookAt(
  Vector(0,0,-5),
  Vector(),
  Vector(0,1,0)
)

function _update(dt)
  if GetKeyDown(KEY_ESCAPE) then
    ExitGame()
  end

  if GetKeyDown("l") then
    toggleLighting = not toggleLighting
  end

  if GetKeyDown(KEY_SPACE) then
    if music:isPlaying() then
      music:pause()
    else
      music:play()
    end
  end

  if GetKeyDown(KEY_RETURN) then
    music:stop()
    music:play()
  end
end

function _render()
  ClearScene(bgColor)
  AmbientColor(bgColor)
  CameraPerspective(62, 0.1, 100)
  EnableLighting(toggleLighting)
  viewMat:bind(VIEW)

  sphere:draw(
    Matrix():rotate(getTime()/4,0,0)
  )
end

function _render2d()
  testFont:drawText(0xFFFFFFFF, [[
    Welcome to the NEON86 basic template!

    Press L to toggle lighting
  ]], 0, 30, 0, 0, FF_NOCLIP)
end
