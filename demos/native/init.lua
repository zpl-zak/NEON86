
local testFont

function _init()
  testFont = Font("Courier New", 48, 16, false)
end

function _update(dt)
  if GetKeyDown(KEY_ESCAPE) then
      ExitGame()
  end
end

function _render()
  ClearScene(20,20,69)
end

function _render2d()
  testFont:drawText(0xFFFFFFFF, "This demo is supposed to be run via NativeDemo app!", 15, 30)
end
