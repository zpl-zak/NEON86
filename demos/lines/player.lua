SPEED = 1
SEND_TIME = 0.05

function setupPlayer()
    player.pos = Vector3()
    player.cam = Matrix()
    player.tank = tanks["local"]
    player.angles = {0,0}
    player.heading = 0
    player.sendTime = 0
end

function player.update(self, dt, net)
    if GetCursorMode() == CURSORMODE_CENTERED then
        mouseDelta = GetMouseDelta()
        self.angles[1] = self.angles[1] + (mouseDelta[1] * dt * 0.15)
        self.angles[2] = self.angles[2] - (mouseDelta[2] * dt * 0.15)
    end

    self.pos = self.pos:lerp(self.tank.pos:neg(), 0.233589)
    self.cam = Matrix()
        :translate(self.pos)
        :rotate(-self.angles[1],0,0)
        :rotate(0,self.angles[2],0)
        :translate(Vector3(0,-20,80))

    local rotMat = Matrix()
    :rotate(-self.heading,0,0)

    local fwd = rotMat:col(3)
    local rhs = rotMat:col(1)

    -- spectator cam
    --[[ self.cam = Matrix():lookAt(
        self.pos:neg()+Vector3(-250,500,-250),
        self.pos:neg(),
        Vector3(0,1,0)
    ) ]]


    self.heading = hh.lerp(self.heading, self.angles[1], 0.1238772)
    self.tank.rot = Matrix():rotate(self.heading+math.rad(90),0,0)

    self.tank.movedir = Vector()

    if GetKey("w") then
        self.tank.movedir = self.tank.movedir + fwd*SPEED*dt
    end
    if GetKey("s") then
        self.tank.movedir = self.tank.movedir - fwd*SPEED*dt
    end
    if GetKey("a") then
        self.tank.movedir = self.tank.movedir - rhs*SPEED*dt
    end
    if GetKey("d") then
        self.tank.movedir = self.tank.movedir + rhs*SPEED*dt
    end

    if GetKey(KEY_SHIFT) then
        self.tank.vel = self.tank.vel:lerp(Vector3(0,--[[ self.tank.vel:y() ]] 0, 0), 0.04221)
    end

    if self.sendTime < time then
        self.sendTime = time + SEND_TIME
        local npos = self.pos:neg()
        net.send(npos:x(), npos:y(), npos:z(), self.heading)
    end
end

