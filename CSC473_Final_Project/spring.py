
import bpy
import numpy as np
import bmesh

from mathutils import Vector

class Particle:
    def __init__(self, position, velocity):
        self.position = position
        self.velocity = velocity
        # 使用字典来存储连接的粒子及其对应的静止长度
        self.stretch_connections = []  #list of connection objects that are stretch connections
        self.shear_connections = []   #list of connection objects that are shear connections
        self.bend_connections = []   #list of connection objects that are bend connections
        self.vNew = Vector((0, 0, 0))
        self.xNew = Vector((0, 0, 0))

    def add_connection(self, particle, spring_type, rest_length):

        if spring_type == 'stretch':
            connection = Connection(particle, spring_type, rest_length)
            self.stretch_connections.append(connection)
        elif spring_type == 'shear':
            connection = Connection(particle, spring_type, rest_length)
            self.shear_connections.append(connection)
        elif spring_type == 'bend':
            connection = Connection(particle, spring_type, rest_length)
            self.bend_connections.append(connection)
    
    def stretch_spring_force (self, connection, stiffness, damping, drag, mass):
        #print(connection.rest_length)
        #print(self.position, connection.particle.position, (self.position - connection.particle.position).length)
        pure_spring_force = stiffness * (connection.rest_length - (self.position - connection.particle.position).length) * (self.position - connection.particle.position).normalized()
        #print((connection.rest_length - (self.position - connection.particle.position).length))
        #print((self.position - connection.particle.position).length)
        damper_force = -damping * (self.velocity - connection.particle.velocity)
        return pure_spring_force + damper_force

    def shear_spring_force (self, connection, stiffness, damping, drag, mass):
        pure_spring_force = stiffness * (connection.rest_length - (self.position - connection.particle.position).length) * (self.position - connection.particle.position).normalized()
        damper_force = -damping * (self.velocity - connection.particle.velocity)
        return pure_spring_force + damper_force

    def __repr__(self): # 用于调试
        return f"Particle(position={self.position}, vNew={self.vNew}, xNew={self.xNew})"


class Connection:
    def __init__(self, particle, spring_type, rest_length):
        self.particle = particle  # 连接的粒子对象
        self.spring_type = spring_type  # 弹簧类型: "stretch", "shear", "bend"
        self.rest_length = rest_length  # 静止长度

    def __repr__(self):
        return f"Connection(particle={self.particle}, spring_type={self.spring_type}, rest_length={self.rest_length})"


class Cloth:


    def __init__(self, name, blender_object, particles, mass, kStretch, kShear, damping, drag, deformation_rate):
        self.name = name
        self.blender_object = blender_object
        self.particles = particles
        self.mass = mass
        self.kStretch = kStretch
        self.kShear = kShear
        self.damping = damping
        self.drag = drag
        self.deformation_rate = deformation_rate
    
    def update_symp_euler(self, dt, pin_group):

        for i, particle in enumerate(self.particles):
            if i not in pin_group:
                total_spring_force = Vector((0, 0, 0))
                for j, Connection in enumerate(particle.stretch_connections):
                    
                    dCurr = (particle.position - Connection.particle.position).length                
                    stiffness = self.update_stiffness(dCurr, 'stretch')  
                        
                    stretch_force = particle.stretch_spring_force(Connection, stiffness, self.damping, self.drag, self.mass)
                    #if i == 90:
                        #print(Connection)
                        #print(force)    
                    total_spring_force += stretch_force
                for Connection in particle.shear_connections:
                    dCurr = (particle.position - Connection.particle.position).length
                    stiffness = self.update_stiffness(dCurr, 'shear')
                    shear_force = particle.shear_spring_force(Connection, stiffness, self.damping, self.drag, self.mass)
                    total_spring_force += shear_force
                
                gravity_force = Vector((0, 0, -9.8)) * self.mass
                total_force = total_spring_force + gravity_force
                
                particle.vNew = particle.velocity + dt * total_force / self.mass
                particle.xNew = particle.position + dt * particle.vNew

        for i, particle in enumerate(self.particles):
            if i not in pin_group:
                particle.velocity = particle.vNew
                particle.position = particle.xNew
             

    def update_stiffness(self, dCurr, stringType):

        if stringType == 'stretch':
            max_stretch_length = (self.deformation_rate * self.particles[0].stretch_connections[0].rest_length) + self.particles[0].stretch_connections[0].rest_length
            if dCurr >= max_stretch_length:
                kNew = self.kStretch * (dCurr/self.particles[0].stretch_connections[0].rest_length)
                return kNew
            else:
                return self.kStretch
        
        if stringType == 'shear':
            max_shear_length = (self.deformation_rate * self.particles[0].shear_connections[0].rest_length) + self.particles[0].shear_connections[0].rest_length
            if dCurr >= max_shear_length:
                kNew = self.kShear * (dCurr/self.particles[0].shear_connections[0].rest_length)
                return kNew
            else:
                return self.kShear





    def update_free_fall(self, dt):
        for particle in self.particles:
            gravity_force = Vector((0, 0, -9.8)) * self.mass
            total_force = gravity_force
            vNew = particle.velocity + dt * total_force / self.mass
            xNew = particle.position + dt * vNew
            particle.velocity = vNew
            particle.position = xNew

def animate_free_fall(cloth, start_frame, end_frame, fps):
    # Set the distance the object should move down in each frame
    dt = 1/fps

    #select the grid obj to animate
    curr_obj = cloth.blender_object
    curr_obj.select_set(True)

    # Insert keyframes
    for frame in range(start_frame, end_frame + 1):
        bpy.context.scene.frame_set(frame)
        for i in range(len(curr_obj.data.vertices)):
            hook_name = f"Hook_{i}"
            hook_modifier = curr_obj.modifiers[hook_name]
            if hook_modifier:
                hook_obj = hook_modifier.object
                #print(cloth.particles[i].position)
                hook_obj.location = cloth.particles[i].position
                #print(hook_obj.location)
                hook_obj.keyframe_insert(data_path="location")

                #update the particles position in the cloth object
                
        cloth.update_free_fall(dt)        

def animate_sym_euler(cloth, start_frame, end_frame, fps, pin_group):
    # Set the distance the object should move down in each frame
    dt = 1/fps

    #select the grid obj to animate
    curr_obj = cloth.blender_object
    curr_obj.select_set(True)

    # Insert keyframes
    for frame in range(start_frame, end_frame + 1):
        bpy.context.scene.frame_set(frame)
        for i in range(len(curr_obj.data.vertices)):
            #skip the pinned vertices
            if i in pin_group:
                continue
            hook_name = f"Hook_{i}"
            hook_modifier = curr_obj.modifiers[hook_name]
            if hook_modifier:
                hook_obj = hook_modifier.object
                #print(cloth.particles[i].position)
                hook_obj.location = cloth.particles[i].position
                #print(hook_obj.location)
                hook_obj.keyframe_insert(data_path="location")

                #update the particles position in the cloth object
                
        cloth.update_symp_euler(dt, pin_group)   

def hook_initialize (blender_object):
    # Create hooks for each vertex in the mesh
    for i in range(len(blender_object.data.vertices)):
        # Select the grid object and enter edit mode
        bpy.context.view_layer.objects.active = blender_object
        # Switch to edit mode
        bpy.ops.object.mode_set(mode='EDIT')
        # Deselect all vertices
        bpy.ops.mesh.select_all(action='DESELECT')

        # Switch to vertex select mode
        bpy.context.tool_settings.mesh_select_mode[0] = True
        bpy.context.tool_settings.mesh_select_mode[1] = False
        bpy.context.tool_settings.mesh_select_mode[2] = False

        # Create a new BMesh
        mesh = bmesh.from_edit_mesh(blender_object.data)

        # Deselect all vertices
        for v in mesh.verts:
            v.select = False

        # Select the vertex
        mesh.verts.ensure_lookup_table()  # Ensure the vertices are accessible by index
        mesh.verts[i].select = True

        # Add a new hook to the selected vertex
        bpy.ops.object.hook_add_newob()
        
        # Rename the hook modifier
        hook_modifier = blender_object.modifiers[-1]  # The newly created hook modifier is the last one in the list
        hook_modifier.name = f"Hook_{i}"
        
        
        
        # Switch back to object mode
        bpy.ops.object.mode_set(mode='OBJECT')
        mesh.free()  # Free the BMesh
        # Deselect all objects
        bpy.ops.object.select_all(action='DESELECT')


#initialize the velocity, position and connections of the particles, e.g, the stretch, shear and bend connections and rest length between each of them
def particle_initialize(blender_object, cloth_width):
    # 获取顶点位置
    vertex_positions = [v.co for v in blender_object.data.vertices]
    #print(vertex_positions)
    # 由于顶点位置是相对于对象的原点，我们需要将它们转换为全局坐标系
    vertex_global_positions = [blender_object.matrix_world @ pos for pos in vertex_positions]

    particles = [Particle(Vector(pos), Vector((0, 0, 0))) for pos in vertex_global_positions]
    #print(particles)

    # 添加连接 
    for i, particle in enumerate(particles):
        
        # 计算当前粒子的连接索引
        current_index = i

        left_stretch_index = i - cloth_width
        right_stretch_index = i + cloth_width
        top_stretch_index = i - 1
        bottom_stretch_index = i + 1

        upLeft_shear_index = i - cloth_width - 1
        upRight_shear_index = i + cloth_width - 1
        downLeft_shear_index = i - cloth_width + 1
        downRight_shear_index = i + cloth_width + 1
        
        left_bend_index = i - 2 * cloth_width
        right_bend_index = i + 2 * cloth_width
        top_bend_index = i - 2
        bottom_bend_index = i + 2
    
        # 添加连接

        if left_stretch_index >= 0 and left_stretch_index < len(particles):
            rest_length = (particle.position - particles[left_stretch_index].position).length
            particle.add_connection(particles[left_stretch_index], 'stretch', rest_length)
            #print(rest_length, particle.position, particles[left_stretch_index].position)

        if right_stretch_index >= 0 and right_stretch_index < len(particles):
            rest_length = (particle.position - particles[right_stretch_index].position).length
            particle.add_connection(particles[right_stretch_index], 'stretch', rest_length)
            #print(rest_length, particle.position, particles[right_stretch_index].position)  

        if top_stretch_index >= 0 and top_stretch_index < len(particles) and i % cloth_width != 0:
            rest_length = (particle.position - particles[top_stretch_index].position).length
            particle.add_connection(particles[top_stretch_index], 'stretch', rest_length)
            #print(rest_length, particle.position, particles[top_stretch_index].position)

        if bottom_stretch_index >= 0 and bottom_stretch_index < len(particles) and i % cloth_width != cloth_width - 1:   
            rest_length = (particle.position - particles[bottom_stretch_index].position).length
            particle.add_connection(particles[bottom_stretch_index], 'stretch', rest_length)
            #print(rest_length, particle.position, particles[bottom_stretch_index].position)

        if upLeft_shear_index >= 0 and upLeft_shear_index < len(particles) and i % cloth_width != 0:
            rest_length = (particle.position - particles[upLeft_shear_index].position).length
            particle.add_connection(particles[upLeft_shear_index], 'shear', rest_length)

        if upRight_shear_index >= 0 and upRight_shear_index < len(particles ) and i % cloth_width != 0:
            rest_length = (particle.position - particles[upRight_shear_index].position).length
            particle.add_connection(particles[upRight_shear_index], 'shear', rest_length)

        if downLeft_shear_index >= 0 and downLeft_shear_index < len(particles) and i % cloth_width != cloth_width - 1:
            rest_length = (particle.position - particles[downLeft_shear_index].position).length
            particle.add_connection(particles[downLeft_shear_index], 'shear', rest_length)

        if downRight_shear_index >= 0 and downRight_shear_index < len(particles) and i % cloth_width != cloth_width - 1:
            rest_length = (particle.position - particles[downRight_shear_index].position).length
            particle.add_connection(particles[downRight_shear_index], 'shear', rest_length)

        if left_bend_index >= 0 and left_bend_index < len(particles):
            rest_length = (particle.position - particles[left_bend_index].position).length
            particle.add_connection(particles[left_bend_index], 'bend', rest_length)
        if right_bend_index >= 0 and right_bend_index < len(particles):
            rest_length = (particle.position - particles[right_bend_index].position).length
            particle.add_connection(particles[right_bend_index], 'bend', rest_length)
        if top_bend_index >= 0 and top_bend_index < len(particles):
            rest_length = (particle.position - particles[top_bend_index].position).length
            particle.add_connection(particles[top_bend_index], 'bend', rest_length)
        if bottom_bend_index >= 0 and bottom_bend_index < len(particles):
            rest_length = (particle.position - particles[bottom_bend_index].position).length
            particle.add_connection(particles[bottom_bend_index], 'bend', rest_length)
    
    return particles

# 确保在对象模式
if bpy.context.object.mode != 'OBJECT':
    bpy.ops.object.mode_set(mode='OBJECT')

# 初始化场景
bpy.ops.object.select_all(action='SELECT')
bpy.ops.object.delete()


fps = 120


cloth_width = 12
cloth_length = 12

# 创建平面
bpy.ops.mesh.primitive_grid_add(x_subdivisions=cloth_width-1, y_subdivisions=cloth_width-1, size=2)
# 将平面提升一定高度
bpy.ops.transform.translate(value=(0, 3, 1))
# 选择网格2对象
plane = bpy.context.object
bpy.context.object.name = "myCloth"
# 获取顶点位置
particles = particle_initialize(plane, cloth_width)
mycloth = Cloth("myCloth", blender_object=plane, particles=particles, mass = 0.1, kStretch=500, kShear=60, damping = 1, drag = 0.0, deformation_rate=0.1)
hook_initialize(mycloth.blender_object)
pin_group1 = [0,11]
animate_sym_euler(mycloth, 1, 1000, fps, pin_group1)

