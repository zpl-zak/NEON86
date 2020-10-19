# Author: Dominik Madarasz <contact@madaraszd.net>

bl_info = {
    "name": "Scene Manager",
    "author": "Dominik Madarasz",
    "version": (1, 1, 0),
    "blender": (2, 90, 1),
    "location": "3D View > View > NEON86",
    "description": "Scene manager for NEON86 Blender projects.",
    "warning": "",
    "wiki_url": "",
    "tracker_url": "",
    "category": "NEON86"}
    
import bpy, csv

from bpy.props import (StringProperty,
                       BoolProperty,
                       IntProperty,
                       FloatProperty,
                       FloatVectorProperty,
                       EnumProperty,
                       PointerProperty,
                       CollectionProperty,
                       )
from bpy.types import (Panel,
                       Operator,
                       UIList,
                       AddonPreferences,
                       PropertyGroup,
                       )

class csvFile(PropertyGroup): 
    name: StringProperty( name="Name", description="A name for this item", default="Untitled")

class NeonSettings(PropertyGroup):    
    path : StringProperty(
        name="",
        description="Path to Directory",
        default="",
        maxlen=1024,
        subtype='FILE_PATH')
        
    drop_old_props : BoolProperty(
        name="Drop existing properties",
        default=False)

class NEON_UL_Files(UIList):
    def draw_item(self, context, layout, data, item, icon, active_data, active_propname, index):
        custom_icon = 'OBJECT_DATAMODE'
        
        if self.layout_type in {'DEFAULT', 'COMPACT'}:
            layout.label(text=item.name, icon=custom_icon)
        elif self.layout_type in {'GRID'}:
            layout.alignment = 'CENTER'
            layout.label(text="", icon=custom_icon)

class NEON_OT_NewItem(Operator):
    bl_idname = "csv_files.new_item"
    bl_label = "Add File"
    
    filePath : bpy.props.StringProperty()
    
    def execute(self, context):
        item = context.scene.csv_files.add()
        item.name = self.filePath
        return{'FINISHED'}

class NEON_OT_DeleteItem(Operator):
    bl_idname = "csv_files.delete_item"
    bl_label = "Remove File"
    
    @classmethod
    def poll(cls, context):
        return context.scene.csv_files
    
    def execute(self, context):
        files = context.scene.csv_files
        fileIdx = context.scene.csv_file_index
        
        files.remove(fileIdx)
        context.scene.csv_file_index = min(max(0, fileIdx - 1), len(files) - 1)

        return{'FINISHED'}

class NEON_OT_MoveItem(Operator):
    bl_idname = "csv_files.move_item"
    bl_label = "Move File"
    
    dir: EnumProperty(items=(('UP', 'Up', ""), ('DOWN', 'Down', ""),))
    
    @classmethod
    def poll(cls, context):
        return context.scene.csv_files
    
    def move_index(self):
        index = bpy.context.scene.csv_file_index
        list_index = len(bpy.context.scene.csv_files) - 1
        new_index = index + (-1 if self.dir == 'UP' else 1)
        bpy.context.scene.csv_file_index = max(0, min(new_index, list_index))
    
    def execute(self, context):
        files = context.scene.csv_files
        fileIdx = context.scene.csv_file_index
        
        cell = fileIdx + (-1 if self.dir == 'UP' else 1)
        files.move(cell, fileIdx)
        self.move_index()

        return{'FINISHED'}

class NEON_OT_ImportCSV(Operator):
    """Imports properties for selected objects / entire scene"""
    bl_idname = "object.import_csv"
    bl_label = ""

    def execute(self, context):
        files = context.scene.csv_files
        objects = bpy.context.selected_objects if len(bpy.context.selected_objects) > 0 else bpy.data.objects

        for filePath in files:
            print("csvFile:", filePath.name)
            fileName = bpy.path.abspath(filePath.name)
            with open( fileName ) as csvfile:
                rdr=csv.DictReader(csvfile,quoting=csv.QUOTE_NONE)
                for row in rdr:
                    meshName = row[rdr.fieldnames[0]]
                    meshes = [obj for obj in objects if obj.name == meshName]
                    
                    if len(meshes) == 0:
                        print("meshName:",meshName," not present!")
                        continue
                        
                    mesh = meshes[0]
                    
                    print("meshName:", meshName)
                    
                    for x in range (1, len(rdr.fieldnames)):  
                        propName =  rdr.fieldnames[x]
                        propValue = row[propName]
                        mesh[propName]=propValue
                        print(" Update meshName: ", meshName, ",propName: ", propName, ", propValue: ", mesh[propName])

                    print("")
    		
        return {'FINISHED'}

class NEON_PT_AddCustomPropertiesPanel(Panel):
    bl_idname = "NEON_PT_AddCustomPropertiesPanel"
    bl_label = "CSV Properties Importer"  # scn.neon_props
    bl_space_type = "VIEW_3D"
    bl_region_type = "UI"
    bl_category = "NEON86"
    bl_context = "objectmode"

    def draw(self, context):
        layout = self.layout
        scn = context.scene
        col = layout.column(align=True)
        col.prop(scn.neon_props, "path", text="")
        
        col = layout.column(align=True)
        row = layout.row()
        col.template_list("NEON_UL_Files", "Files", scn, "csv_files", scn, "csv_file_index")
        row.operator('csv_files.new_item', text='Add').filePath = scn.neon_props.path
        row.operator('csv_files.delete_item', text='Remove')
        row.operator('csv_files.move_item', text='Up').dir = 'UP'
        row.operator('csv_files.move_item', text='Down').dir = 'DOWN'
        
        col = layout.column(align=True)
        col.operator("object.import_csv", text="Import")

class NEON_OT_ReplaceProps(Operator):
    """Copies properties of active object to all selected objects"""
    bl_idname = "object.replace_props"
    bl_label = ""
    
    drop_old_props : BoolProperty()

    def execute(self, context):
        selected = context.selected_objects
        active = context.active_object
        
        for object in selected:
            if object.name == active.name:
                continue
            
            if self.drop_old_props is True:
                for prop in object.items():
                    del object[prop[0]]
            
            for prop in active.items():
                object[prop[0]] = prop[1]            
                        
        return {'FINISHED'}

class NEON_PT_ReplaceProperties(Panel):
    bl_idname = "NEON_PT_ReplaceProperties"
    bl_label = "Replace Properties"
    bl_space_type = "VIEW_3D"
    bl_region_type = "UI"
    bl_category = "NEON86"
    bl_context = "objectmode"

    def draw(self, context):
        layout = self.layout
        scn = context.scene
        col = layout.column(align=True)
        col.prop(scn.neon_props, "drop_old_props")
        op = col.operator("object.replace_props", text="Copy Active to Selected")
        op.drop_old_props = scn.neon_props.drop_old_props


classes = (
    csvFile,
    NEON_OT_NewItem,
    NEON_OT_DeleteItem,
    NEON_OT_MoveItem,
    NEON_UL_Files,
    NEON_OT_ImportCSV,
    NEON_OT_ReplaceProps,
    NeonSettings,
    NEON_PT_AddCustomPropertiesPanel,
    NEON_PT_ReplaceProperties,
)

def register():
    from bpy.utils import register_class
    for cls in classes:
        register_class(cls)
    bpy.types.Scene.neon_props = PointerProperty(type=NeonSettings)
    bpy.types.Scene.csv_files = CollectionProperty(type=csvFile)
    bpy.types.Scene.csv_file_index = IntProperty(name="File index", default = 0)

def unregister():
    from bpy.utils import unregister_class
    for cls in reversed(classes):
        unregister_class(cls)
    del bpy.types.Scene.neon_props

if __name__ == "__main__":
    register()



