local Level = require "Level"
local level1 = Level("level1")

function _update(dt)
    if GetKeyDown(KEY_ESCAPE) then
        ExitGame()
    end

    if GetMouse(MOUSE_RIGHT_BUTTON) then
        ShowCursor(false)
        SetCursorMode(CURSORMODE_CENTERED)
    else
        ShowCursor(true)
        SetCursorMode(CURSORMODE_DEFAULT)
    end

    level1:update(dt)
end

function _render()
    level1:render()
end
