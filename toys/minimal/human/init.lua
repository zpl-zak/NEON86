local Human = {}
Human.__index = Human

function Human.new(mdl)
    local self = setmetatable({}, Human)
    self.humanModel = mdl
    self.humanFrame = 1
    self.humanFrames = #self.humanModel:getMeshes()
    self.frameCounter = 1

    return self
end

function Human.update(self)
    if self.frameCounter == 6 then
        self.humanFrame += 1

        if self.humanFrame >= self.humanFrames then
            self.humanFrame = 1
        end
        self.frameCounter = 0
    end

    self.frameCounter += 1
end

function Human.draw(self, wmat)
    self.humanModel:drawSubset(self.humanFrame, wmat)
end

return Human.new