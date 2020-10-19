# Inspired by: http://github.com/MarioDelgadoSr/AddBlenderCustomPropertiesFromCSV
# Author: Dominik Madarasz <contact@madaraszd.net>

bl_info = {
    "name": "CSV Properties Importer",
    "author": "Dominik Madarasz",
    "version": (1, 0, 0),
    "blender": (2, 90, 1),
    "location": "3D View > View > CSV",
    "description": "Ingest multiple CSV files and distribute custom properties to objects.",
    "warning": "",
    "wiki_url": "",
    "tracker_url": "",
    "category": "Object"}
    
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

class addPropsSettings(PropertyGroup):
    
    path : StringProperty(
        name="",
        description="Path to Directory",
        default="",
        maxlen=1024,
        subtype='FILE_PATH')

class CSVADD_UL_Files(UIList):
    def draw_item(self, context, layout, data, item, icon, active_data, active_propname, index):
        custom_icon = 'OBJECT_DATAMODE'
        
        if self.layout_type in {'DEFAULT', 'COMPACT'}:
            layout.label(text=item.name, icon=custom_icon)
        elif self.layout_type in {'GRID'}:
            layout.alignment = 'CENTER'
            layout.label(text="", icon=custom_icon)

class CSVADD_OT_NewItem(Operator):
    bl_idname = "csv_files.new_item"
    bl_label = "Add File"
    
    filePath : bpy.props.StringProperty()
    
    def execute(self, context):
        item = context.scene.csv_files.add()
        item.name = self.filePath
        return{'FINISHED'}

class CSVADD_OT_DeleteItem(Operator):
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

class CSVADD_OT_MoveItem(Operator):
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

class processCustom(Operator):
  
    bl_idname = "object.process_custom"
    bl_label = ""

    def execute(self, context):
        files = context.scene.csv_files

        # https://docs.python.org/3/library/csv.html
        for filePath in files:
            print("******************************** csvFile:", filePath.name ,"********************************************")
            fileName = bpy.path.abspath(filePath.name)
            with open( fileName ) as csvfile:
                rdr=csv.DictReader(csvfile,quoting=csv.QUOTE_NONE)
                for row in rdr:
                    meshName = row[rdr.fieldnames[0]]
                    meshes = [obj for obj in bpy.data.objects if obj.name == meshName]
                    
                    if length(meshes) == 0:
                        print("meshName:",meshName," not present!")
                        continue
                        
                    mesh = meshes[0]
                    
                    print("******************************** meshName:", meshName ,"********************************************")
                    
                    for x in range (1, len(rdr.fieldnames)):  
                        propName =  rdr.fieldnames[x]
                        propValue = row[propName]
            			
    					#Custom Property Assigned to Object, this assures userData on ThreeJS is assigned to groups as well
                        mesh[propName]=propValue
                        print(" Update meshName: ", meshName, ",propName: ", propName, ", propValue: ", mesh[propName])
                    
                    print(" properties after assignment(s): ", bpy.data.objects[meshName].items())
                    print("")
    		
        return {'FINISHED'}

# ---------------------------------------------------------------------------------
#  Customize Tool Panel and add file selector and check box for sanitize option
# ---------------------------------------------------------------------------------

class CSVADD_PT_addCustomPropertiesPanel(Panel):
    bl_idname = "CSVADD_PT_addCustomPropertiesPanel"
    bl_label = "Import CSV data"  # scn.csv_to_custom_props
    bl_space_type = "VIEW_3D"
    bl_region_type = "UI"
    bl_category = "CSV"
    bl_context = "objectmode"

    def draw(self, context):
        layout = self.layout
        scn = context.scene
        col = layout.column(align=True)
        col.prop(scn.csv_to_custom_props, "path", text="")
        
        col = layout.column(align=True)
        row = layout.row()
        col.template_list("CSVADD_UL_Files", "Files", scn, "csv_files", scn, "csv_file_index")
        row.operator('csv_files.new_item', text='Add').filePath = scn.csv_to_custom_props.path
        row.operator('csv_files.delete_item', text='Remove')
        row.operator('csv_files.move_item', text='Up').dir = 'UP'
        row.operator('csv_files.move_item', text='Down').dir = 'DOWN'
        
        col = layout.column(align=True)
        col.operator("object.process_custom", text="Import")
        
# ------------------------------------------------------------------------
#    register and unregister functions
# ------------------------------------------------------------------------

classes = (
    csvFile,
    CSVADD_OT_NewItem,
    CSVADD_OT_DeleteItem,
    CSVADD_OT_MoveItem,
    CSVADD_UL_Files,
    processCustom,
    addPropsSettings,
    CSVADD_PT_addCustomPropertiesPanel
)

def register():
    from bpy.utils import register_class
    for cls in classes:
        register_class(cls)
    bpy.types.Scene.csv_to_custom_props = PointerProperty(type=addPropsSettings)
    bpy.types.Scene.csv_files = CollectionProperty(type=csvFile)
    bpy.types.Scene.csv_file_index = IntProperty(name="File index", default = 0)

def unregister():
    from bpy.utils import unregister_class
    for cls in reversed(classes):
        unregister_class(cls)
    del bpy.types.Scene.csv_to_custom_props

if __name__ == "__main__":
    register()



