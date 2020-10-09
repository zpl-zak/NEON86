local Level = require "Level"
local fsm = require "fsm"

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
  initial = {state = "menu", event = "init_menu"},
  events = {
    {name = "cycle_menu", from = "level2", to = "level1"},
    {name = "cycle_menu", from = "level1", to = "level2"},
    {name = "cycle_menu", from = "level2", to = "menu"},
    {name = "cycle_menu", from = "menu", to = "level1"},
  },
  callbacks = {
    on_cycle_menu = function (self, event, from, to)
      if to == "level2" then
        self.level = levels[2]
      elseif to == "menu" then
        LogString("menu")
        self.level = levels[3]
      elseif to == "level1" then
        self.level = levels[1]
      end
    end,
    on_init_menu = function (self)
      self.level = levels[3]
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
    menu.cycle_menu()
  end

  menu.level:update(dt)
end

function _render()
  menu.level:render()
end

local testFont = Font("Arial", 18, 1, false)

function _render2d()
  testFont:drawText(0xFFFFFFFF, menu.current, 15, 30)
end
