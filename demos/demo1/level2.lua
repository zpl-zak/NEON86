local bgColor = Color(69,20,20)

return {
  name = "Level 2",
  camera = {
    position = Vector(0,0,-5),
    fov = 62,
    moveSpeed = 15.0,
    mouseSensitivity = 0.15,
    clips = {
      near = 0.1,
      far = 100
    }
  },
  scenery = {
    backdropColor = bgColor,
    ambientColor = bgColor,
    lightOffset = 0,
    lightsEnabled = true
  },
  models = {
    sphere = {
      filename = "sphere.fbx",
      loadMaterials = true,
      optimizeMesh = false
    }
  },
  lights = {
    sun = {
      kind = "directional",
      enabled = true,
      direction = Vector3(1,-1,1),
    }
  },
  nodes = {
    empty2 = {
      onUpdate = function (self, dt)
        LogString("updating from empty2 ...")
      end
    }
  }
}
