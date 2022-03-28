local dll = require "sampleplugin"

function _init()
  ShowMessage("Field test", tostring(dll.answer))
  ShowMessage("Method test", tostring(dll.test()))

  local w, h = dll.interopTest()
  ShowMessage("Engine interop test", "w: " .. w .. " h: " .. h)

  ShowMessage("Engine cross-Lua obj test", vec2str(dll.randvec()))
  ExitGame()
end