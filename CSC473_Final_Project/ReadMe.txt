README for Cloth Simulation in Blender 4.0

Overview
    This program is a cloth simulation script for Blender 4.0, designed to demonstrate the physics of cloth hanging under the influence of gravity. 
    It uses a mass-spring model with adjustable parameters for stiffness and damping. This simulation also includes the ability to pin certain vertices 
    to create a fixed point from which the cloth can hang.

Requirements
    Blender 4.0
    Python 3.7 or higher (bundled with Blender)
    mathutils (included with Blender)
Setup Instructions
    Open Blender 4.0 and switch to the scripting tab.
    Open the spring.py file through the text editor in Blender.
    Running the Simulation

To run the script, press the 'Run Script' button in the text editor header or press Alt + P with the cursor in the text editor window where the script is open.

Script Breakdown
    fps: In line 326, use to calculate time step by 1/fps
    Mesh Size: In line 329&330 (cloth_width and cloth_length), determines the size of the grid mesh representing the cloth. MUST BE N * N.
    Parameter Adjustments: In line 341, you can adjust physical properties like mass (mass), stretch stiffness (kStretch), shear stiffness (kShear), damping (damping), and drag (drag)(never used in this project). Additionally, the deformation_rate can be set to adjust the threshold for the spring stiffness adjustment in the simulation.
    pin group: In line 343, contains the index of pinned particles
    Animation Creation: In line 344, animating the cloth,  you can adjust the animation by changing the start frame, end frame, fps, and pin group

Customization
To customize the simulation:
    Modify the cloth_width and cloth_length(n * n) variables to change the mesh size.
    Adjust mass, kStretch, kShear, dampingto explore different cloth behaviors.
    Change the vertices in the pin_group list to fix different points of the cloth mesh.
Notes
    The file needs to be run within Blender's environment for the Python API to access the necessary functions.
    The Cloth object is linked to a Blender object, which should be a mesh in your scene.
    The hook_initialize function must be run to create hook modifiers for each vertex of the mesh for animation purposes.
    Make sure to save your Blender project before running the script to prevent loss of any prior work in case of unexpected behavior.