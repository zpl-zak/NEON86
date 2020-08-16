music = Music("music.ogg")
music:play()
music:setVolume(80)
size = music:getTotalSize()

require "helpers".global()

function _update(dt)
  if GetKeyDown(KEY_ESCAPE) then
    ExitGame()
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

res = GetResolution()
pos = 0

function _render ()
  ClearScene(0x0)
  pos = lerp(pos, music:getPosition() / size, 0.01)
  DrawQuad(0, lerp(0, res[1], pos), 0, res[2], 0xFFFFFFFF)
end
