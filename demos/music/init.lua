music = Music("music.ogg")
music:play()
music:setVolume(80)

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

function _render()
  ClearScene(0x0)
end