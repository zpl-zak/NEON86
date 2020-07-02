local dll = require "sampleplugin"

function _init()
  ShowMessage("Field test", tostring(dll.answer))
  ShowMessage("Method test", tostring(dll.test()))
  ExitGame()
end