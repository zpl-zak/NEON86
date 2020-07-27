counter = 0

if LoadState() ~= nil then
  counter = tonumber(LoadState())+1
end

ShowMessage("Counter demo", "We're at: " .. counter .. " now!")

SaveState(tostring(counter))
ExitGame()