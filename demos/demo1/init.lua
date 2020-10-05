local Level = require "Level"
local fsm = require "fsm"

local levelID = 1
local levels = {
    Level("level1"),
    Level("level2"),
    {
        update = function ()
        end,
        render = function ()
            ClearScene(0,0,0)
        end,
    },
}

local menu = fsm.create({
    initial = "level1",
    events = {
        {name = "level1", from = "level2", to = "level1"},
        {name = "level2", from = "level1", to = "level2"},

        {name = "menu", from = "*", to = "menu"},
        {name = "level1", from = "menu", to = "level1"},
    },
    callbacks = {
        on_level1 = function ()
            levelID = 1
        end,
        on_level2 = function ()
            levelID = 2
        end,
        on_menu = function ()
            LogString("menu")
            levelID = 3
        end
    }
})

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

    if GetKeyDown(KEY_SPACE) then
        if menu.current == "level1" then
            menu.level2()
        elseif menu.current == "level2" then
            menu.menu()
        elseif menu.current == "menu" then
            menu.level1()
        end
    end

    levels[levelID]:update(dt)
end

function _render()
    levels[levelID]:render()
end

local testFont = Font("Arial", 18, 1, false)

function _render2d()
    testFont:drawText(0xFFFFFFFF, menu.current, 15, 30)
end
