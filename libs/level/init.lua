local class = require "class"
local helpers = require "helpers"

local function opt(field, default)
    if field == nil then
        return default
    end

    return field
end

class "Level" {
    __init__ = function (self, filename)
        local data = require(filename)

        self.name = data.name
        self:setupCamera(data.camera)
        self:setupScenery(data.scenery)
        self:setupModels(data.models)
        self:setupLights(data.lights)
        self:setupNodes(data.nodes)
    end,

    update = function (self, dt)
        self.camera.controller:update(dt)

        for _, node in pairs(self.nodes) do
            self:doFunctor(node, "onUpdate", dt)
        end

        for _, light in pairs(self.lights) do
            self:doFunctor(light, "onUpdate", dt)
        end

        for _, model in pairs(self.models) do
            self:doFunctor(model, "onUpdate", dt)
        end
    end,

    render = function (self)
        ClearScene(self.scenery.backdropColor)
        AmbientColor(self.scenery.ambientColor)
        EnableLighting(self.scenery.lightsEnabled)
        CameraPerspective(self.camera.fov, self.camera.clips.near, self.camera.clips.far)
        self.camera.controller.mat:bind(VIEW)

        for _, light in pairs(self.lights) do
            light.light:enable(light.enabled, self.scenery.lightOffset + light.index)
            self:doFunctor(light, "onRender")
        end

        for _, model in pairs(self.models) do
            if model.enabled == true then
                self:loadModel(model)
                model.model:draw(model.transform)
            end
            self:doFunctor(model, "onRender")
        end
    end,

    setupCamera = function (self, camera)
        local Camera

        if camera.controller == nil then
            Camera = require "camera"
        else
            Camera = camera.controller
        end
        local position = opt(camera.position, Vector3(0,0,0))
        local angles = opt(camera.angles, {0.0, 0.0})
        local moveSpeed = opt(camera.moveSpeed, 15.0)
        local mouseSensitivity = opt(camera.mouseSensitivity, 0.15)
        local fov = opt(camera.fov, 62)
        local clips = opt(camera.clips, {})

        if clips.near == nil then clips.near = 0.1 end
        if clips.far == nil then clips.far = 100 end

        self.camera = { controller = Camera(position, angles) }
        self.camera.controller.sensitivity = mouseSensitivity
        self.camera.controller.speed = moveSpeed
        self.camera.fov = fov
        self.camera.clips = clips
    end,

    setupScenery = function (self, scenery)
        if scenery == nil then scenery = {} end
        self.scenery = helpers.deepcopy(scenery)

        if self.scenery.backdropColor == nil then self.scenery.backdropColor = Color(20,20,20) end
        if self.scenery.ambientColor  == nil then self.scenery.ambientColor = Color(20,20,20) end
        if self.scenery.lightsEnabled == nil then self.scenery.lightsEnabled = true end
    end,

    setupModels = function (self, rawModels)
        self.models = {}

        for name, data in pairs(rawModels) do
            self.models[name] = helpers.deepcopy(data)
            if self.models[name].enabled == nil then self.models[name].enabled = true end

            if self.models[name].forceLoad == true then
                self:loadModel(model[name])
            end
        end
    end,

    setupLights = function (self, rawLights)
        self.lights = {}
        local index = 0

        for name, data in pairs(rawLights) do
            self.lights[name] = helpers.deepcopy(data)
            self:loadLight(self.lights[name], index)
            index = index + 1
        end
    end,

    setupNodes = function (self, rawNodes)
        self.nodes = {}

        for name, data in pairs(rawNodes) do
            self.nodes[name] = helpers.deepcopy(data)
            self:loadNode(self.nodes[name])
        end
    end,

    -- loaders

    loadModel = function (self, model)
        if model._lazyLoad == true then return end
        model.model = Model(model.filename, model.loadMaterials, model.optimizeMesh)
        if model.transform == nil then model.transform = Matrix() end
        model._lazyLoad = true
    end,

    loadLight = function (self, light, index)
        light.index = index
        light.light = Light(index)

        if light.diffuse ~= nil then
            light.light:setDiffuse(light.diffuse)
        end

        if light.ambient ~= nil then
            light.light:setAmbient(light.ambient)
        end

        if light.specular ~= nil then
            light.light:setSpecular(light.specular)
        end

        if light.kind == "directional" then
            light.light:setType(LIGHTKIND_DIRECTIONAL)
            light.light:setDirection(opt(light.direction, Vector3(1,1,-1)))
            light.light:setAttenuation(1,0,0)
        elseif light.kind == "point" then
            light.light:setType(LIGHTKIND_POINT)
            light.light:setPosition(opt(light.position, Vector3(0,0,0)))
            light.attenuation = getAtten(light.attenuation)
            light.light:setAttenuation(light.attenuation.constant, light.attenuation.linear, light.attenuation.quadratic)
            light.light:setRange(opt(light.range, 10.0))
        elseif light.kind == "spot" then
            light.light:setType(LIGHTKIND_SPOT)
            light.light:setPosition(opt(light.position, Vector3(0,0,0)))
            light.light:setDirection(opt(light.direction, Vector3(0,-1,0)))
            light.attenuation = getAtten(light.attenuation)
            light.light:setAttenuation(light.attenuation.constant, light.attenuation.linear, light.attenuation.quadratic)
            light.light:setRange(opt(light.range, 10.0))
            light.light:setFalloff(opt(light.falloff, 1.0))
            light.light:setOuterAngle(opt(light.outerAngle, math.rad(30)))
            light.light:setInnerAngle(opt(light.innerAngle, math.rad(15)))
        else
            LogString("Unsupported light type: ".. light.kind .. " !")
        end

        if light.enabled == nil then light.enabled = true end
    end,

    loadNode = function (self, node)
    end,

    -- helpers

    doFunctor = function (self, ent, functionName, arg0)
        if ent[functionName] ~= nil then
            ent[functionName](ent, arg0)
        end
    end,

    getAtten = function (atten)
        local a = opt(atten, {})

        if a.constant == nil then a.constant = 0.0 end
        if a.linear == nil then a.linear = 1.0 end
        if a.quadratic == nil then a.quadratic = 0.0 end

        return a
    end

}

return Level
