# creates a new scene with model frames exploded into meshes

import bpy
import os


view_layer = bpy.context.view_layer

obj_active = view_layer.objects.active
selection = bpy.context.selected_objects

bpy.ops.object.select_all(action='DESELECT')

for obj in selection:

    obj.select_set(True)

    view_layer.objects.active = obj

    name = bpy.path.clean_name(obj.name)

    obj.select_set(False)

    print("written:", name)
    
    col = bpy.data.collections.new(name + "_anim")
    bpy.context.scene.collection.children.link(col)

    frame = obj.copy()
    frame.animation_data_clear()
    frame.modifiers.clear()
    frame.constraints.clear()
    col.objects.link(frame)

view_layer.objects.active = obj_active

for obj in selection:
    obj.select_set(True)
