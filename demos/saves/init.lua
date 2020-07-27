state = {
  counter = 0,
  hello = function (title)
    ShowMessage(title, "We're at: " .. state.counter .. " now!")
  end
}

require "helpers".global()

if LoadState() ~= nil then
  state = decode(LoadState())
  state.counter = state.counter + 1
end

state.hello("Counter demo")

SaveState(encode(state))
ExitGame()