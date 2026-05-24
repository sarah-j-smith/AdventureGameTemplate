# Creating a Nav Mesh for a Level

To create a nav mesh for our 2D character to path around the level, and avoid obstacles we need to add a navigation mesh.

The process is:

* Create a static mesh, 4 units deep that follows the shape of the area the character can walk on
* Edit that static mesh, adding complex collision shape to set it up to use in the game
* In the level, add a navigation bounding volume this should generate the nav mesh

There is also some once off set up required for the navigation system. Let's get into it.

# 1. Pen Tool to Create the Static Mesh

![Brush mode](./images/activate-brush-editing.png)
_Activate brush editing in the viewport_

Using the Brush Mode in the view port to create the static mesh. Follow the process in my video, and since the 
areas the characters can walk are often long and thin, start in the middle and avoid long continuous boundaries.

* Note: draw the shape counter-clockwise
  * Starting from the middle-top

[![Brush mode mesh creation](https://img.youtube.com/vi/s8MARD3kJUk/0.jpg)](https://www.youtube.com/watch?v=s8MARD3kJUk)

* Ensure the view mode is **"Top" (not Perspective)** and the rendering in the editor is unlit
* Ensure the environment sprite, eg `TowerBackground_Sprite` is visible in the viewport
* Click the dropdown for the editing mode and switch to <b>Brush Editing Mode</b>.

We use the "Pen" tool which works by pointing with the mouse; and tapping space bar to create points. These points form
joined edges around the edges of the navigable area. Then you close that by pressing enter. See the above video for a
demonstration of this.

* Choose "pen" and make the settings "Auto extrude: true"
* Extrude depth 4 - the rest leave unchecked

* Click apply
* Move the mouse into the editor area, and click once to get focus
* Then move the mouse to a position on the edge of your navigable area
* Press the space bar to create a point there
* Move the mouse again, press the space bar again
* Repeat until the whole shape is outlined except for the last connection
* Press enter to close the shape

![Brush in scene](./images/brush-in-scene.png)

# 2. Convert the Brush to a Mesh

The brush created in the previous step should appear in the scene, aligned
with your artwork. The brush will be grey and a sort of checkerboard pattern. 

Convert the brush to a mesh by opening the
advanced panel and clicking the _Create Static Mesh_ button.

* Click <b>Create Static Mesh</b>

![convert to static mesh](./images/mesh_4_convert_to_sm.png)

When this button is clicked, you get a dialog to save the mesh object in your games content. Save it to the meshes folder. Use `SM_` prefix for all static meshes. Its also a good idea for any non-trivial game to have a folder in there for each room:

* `Content/Meshes/Lounge/SM_Lounge_Nav`

After you save the mesh, the brush will be gone from the scene and will be replaced by a 
static mesh, linked to the place you saved it.

# 3. Set up the Mesh for the Game

Navigate to where the mesh was saved in the content drawer, eg `Content/Meshes/Lounge/SM_Lounge_Nav` and double-click to open it. 

Choose <b>Top</b> in the viewport and confirm the mesh is the correct shape. Now we set up
the collisions for the mesh. For many nav meshes it is likely a "re-entrant" shape, (like a "U" shape) so its
important to do this correctly. 

* Click the "Collision" menu option at the top tool bar
* Choose the "Auto Convex Collision" option

![Mesh setup](./images/collision-menu.png)

* A tool dialog pops up at the bottom right on the details panel
  * The settings of 
    * Hull count 4
    * Max hull verts 16
    * Hull precision 100,000 
  * Seem to work ok. If the shape is very re-entrant and the collision mesh looks bad, try increasing the Hull count.

![collision creation](./images/mesh_6_convex_collision.png)

* On the tool dialog click "Apply" and dismiss the dialog.
* Now on the mesh "Collision" section of the details panel choose as follows:

![presets](./images/mesh_8_presets.png)

# 4. Complex Collision

* Collision complexity: Use complex collision as simple

There is a good explanation of what this is doing [on the Unreal website](https://dev.epicgames.com/documentation/en-us/unreal-engine/simple-versus-complex-collision-in-unreal-engine?application_version=5.5) but basically for physics bodies in an unreal
scene you can have two different collision shapes: a complex set and a simple set. Its cheaper performance-wise to use
the simple queries (such as did this click or tap hit an object) than the complex queries.

![Complex as Simple](./images/complex-as-simple.png)

However for complex shapes, a simple convex hull (as though a bed-sheet was wrapped
over it) would be bad because you would miss objects that could be seen behind the shape. 

So here we tell Unreal to use the complex collision in place of the simple one.

# 5. Create the Nav Mesh In the Level 

* Before adding the mesh check the [nav mesh agent settings]. 
  * This only needs to be done once, and then you can create a nav mesh for each new room
  * Its complex, sensitive and fiddly to take time to get it right

[nav mesh agent settings]: ./NavMeshAgentSettings.md

* Back in the level editor, make sure the Mode Switcher dropdown top left is set to "Selection Mode"
* Click into the "Place Actors" tab and filter for "Nav Mesh"
  * then drag the _Nav Mesh Bounds Volume_ into the level
* Click on the Nav Mesh Bounds Volume in the Outliner and set its location transform to zeros
  * Set the scale transforms so that the Nav Mesh Bounds volume's yellow box neatly encloses the background
    * For the `TowerExterior_Sprite` this is `X: 0.765, Y: 2.435, Z: 1.0`
* With luck you'll see a green nav mesh appear in the scene as below
  * The `Outliner` tab will show a `Recast-NavMesh` object in the scene

![Nav mesh bounds](./images/recast-nav-mesh.png)
_The Recast nav mesh appears in the scene after the bounds volume is added_

* Click on the Nav Mesh Bounds Volume in the Outliner and set its transform to zeros
  * Change the Transform Location "Z" to -3
* Click on the `SM_TowerExterior_Nav` static mesh and move it to `Z: -3` as well
  * You should see a navigation mesh in the scene, coloured green
* If its not there press "P" to turn on nav mesh visualisation

![Nav mesh in scene](./images/nav)