# Author: Dominik Madarasz <contact@madaraszd.net>

bl_info = {
    "name": "Scene Manager",
    "author": "Dominik Madarasz",
    "version": (1, 2, 1),
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
    name: StringProperty( name="Name", description="CSV file", default="untitled.csv")

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
        
    game_exe : StringProperty(
        name="",
        description="Path to Directory",
        default="w:/neon86/build/debug/player.exe",
        maxlen=1024,
        subtype='FILE_PATH')
        
    game_dir : StringProperty(
        name="",
        description="Path to Directory",
        default="w:/neon86",
        maxlen=1024,
        subtype='DIR_PATH')
        
    game_data : StringProperty(
        name="",
        description="Path to Directory",
        default="w:/neon86/build/deploy/data",
        maxlen=1024,
        subtype='DIR_PATH')
                
    csv_files : CollectionProperty(type=csvFile)
    csv_file_index : IntProperty(name="File index", default = 0)

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
        props = context.scene.neon_props
        item = props.csv_files.add()
        item.name = self.filePath
        return{'FINISHED'}

class NEON_OT_DeleteItem(Operator):
    bl_idname = "csv_files.delete_item"
    bl_label = "Remove File"
    
    @classmethod
    def poll(cls, context):
        return context.scene.csv_files
    
    def execute(self, context):
        props = context.scene.neon_props
        files = props.csv_files
        fileIdx = props.csv_file_index
        
        files.remove(fileIdx)
        props.csv_file_index = min(max(0, fileIdx - 1), len(files) - 1)

        return{'FINISHED'}

class NEON_OT_MoveItem(Operator):
    bl_idname = "csv_files.move_item"
    bl_label = "Move File"
    
    dir: EnumProperty(items=(('UP', 'Up', ""), ('DOWN', 'Down', ""),))
    
    @classmethod
    def poll(cls, context):
        return context.scene.csv_files
    
    def move_index(self, props):
        index = props.csv_file_index
        list_index = len(props.csv_files) - 1
        new_index = index + (-1 if self.dir == 'UP' else 1)
        props.csv_file_index = max(0, min(new_index, list_index))
    
    def execute(self, context):
        props = context.scene.neon_props
        files = props.csv_files
        fileIdx = props.csv_file_index
        
        cell = fileIdx + (-1 if self.dir == 'UP' else 1)
        files.move(cell, fileIdx)
        self.move_index(props)

        return{'FINISHED'}

class NEON_OT_ImportCSV(Operator):
    """Imports properties for selected objects / entire scene"""
    bl_idname = "object.import_csv"
    bl_label = ""

    def execute(self, context):
        props = context.scene.neon_props
        files = props.csv_files
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

class NEON_PT_ImportCSVFiles(Panel):
    bl_idname = "NEON_PT_ImportCSVFiles"
    bl_label = "CSV Properties Importer"  # scn.neon_props
    bl_space_type = "VIEW_3D"
    bl_region_type = "UI"
    bl_category = "NEON86"
    bl_context = "objectmode"

    def draw(self, context):
        layout = self.layout
        scn = context.scene
        props = context.scene.neon_props
        col = layout.column(align=True)
        col.prop(scn.neon_props, "path", text="")
        
        col = layout.column(align=True)
        row = layout.row()
        col.template_list("NEON_UL_Files", "Files", props, "csv_files", props, "csv_file_index")
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


class NEON_PT_GameLauncher(Panel):
    bl_idname = "NEON_PT_GameLauncher"
    bl_label = "Game Launcher"
    bl_space_type = "VIEW_3D"
    bl_region_type = "UI"
    bl_category = "NEON86"
    bl_context = "objectmode"

    def draw(self, context):
        layout = self.layout
        scn = context.scene
        col = layout.column(align=True)
        col.prop(scn.neon_props, "game_exe", text="EXE")
        col.prop(scn.neon_props, "game_dir", text="Workdir")
        col.prop(scn.neon_props, "game_data", text="Data")
        col = layout.column(align=True)
        col.operator("object.launch_game", text="Launch Game ")


class NEON_OT_LaunchGame(Operator):
    bl_idname = "object.launch_game"
    bl_label = ""
    
    def execute(self, context):
        import subprocess
        props = context.scene.neon_props
        
        DETACHED_PROCESS = 0x00000008
        results = subprocess.Popen([props.game_exe, props.game_data],
                                   close_fds=True, cwd=props.game_dir, creationflags=DETACHED_PROCESS)
        return {'FINISHED'} 

classes = (
    csvFile,
    NEON_OT_NewItem,
    NEON_OT_DeleteItem,
    NEON_OT_MoveItem,
    NEON_UL_Files,
    NEON_OT_ImportCSV,
    NEON_OT_ReplaceProps,
    NEON_OT_LaunchGame,
    NeonSettings,
    NEON_PT_GameLauncher,
    NEON_PT_ImportCSVFiles,
    NEON_PT_ReplaceProperties,
)

def register():
    from bpy.utils import register_class
    for cls in classes:
        register_class(cls)
    bpy.types.Scene.neon_props = PointerProperty(type=NeonSettings)

def unregister():
    from bpy.utils import unregister_class
    for cls in reversed(classes):
        unregister_class(cls)
    del bpy.types.Scene.neon_props

if __name__ == "__main__":
    register()



