state = {
  counter = 0
}

require "helpers".global()

if LoadState() ~= nil then
  state = decode(LoadState())
  state.counter = state.counter + 1
end

ShowMessage("Counter demo", "We're at: " .. state.counter .. " now!")

SaveState(encode(state))
ExitGame()