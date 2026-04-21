# How To

This is the process for creating a game with this plugin. First ensure its installed in your Unreal account.

## Create a Game and Main Level

* Open Unreal and from the welcome wizard or File > New Project choose "Blank" Game
    - Name it "MyAdventure" and choose "C++"
      - **Throughout, where you see "MyAdventure" replace with whatever your game is called**
    - click "Create"
* On creation you see a weird map - ignore it as we create a blank level
    - Right-click in Content Browser and choose "Create: New Folder"
      - Create each of these
        - MyAdventure - top levels folder, inside that create:
          - Blueprints
          - Hotspots
          - Materials
          - Meshes
          - Levels
          - StringTables
          - Textures
        - Inside textures create:
          - Environments
          - Items
          - MainCharacter
          - UserInterface

![Folder setup](images/folder-layout.png) 

* File > New Level "MainLevel" 
  * Save it into your new folder `MyAdventure/Content/Levels`

## Project Setup
* Edit > Project Settings > Maps & Modes
    - Set "MainLevel" to be the Editor Startup map and Game Default map
* Add copyright notice and other info in the 
    - Edit > Project Settings > Project > Description

## Setup for Pixel Art
* Motion blur off
* Auto exposure turn off
* Anti-aliasing off - for pixel art

* See this [Setup for 2D Guide] for more details.

[Setup for 2D Guide]: SetupFor2D/Setup.md

## Paper2D setup

* Check Edit > Plugins
    - Check Paper2D is enabled and context menu in the content draw shows Paper2D items
* Edit `ProjectDir/Source/ProjName/ProjName.Build.cs`
    - Include `"Paper2D", "EnhancedInput"` in the array of `PublicDependencyModuleNames`

## Import and sprite creation scale for small sprites

If you have very small sprites for your character (less than 10 pixels) may need to change scale

* Pixels-per-Unreal-unit - eg 0.5 (instead of 1.0)
  * Under Editor - Paper2D - Import
  * Effectively scales up the pixel art
  * Needed for movements systems
  * Needs to approximate human measurements

# Import Assets

A word about Unreal Engine 2D Axes - these are confusing, and the "Top" view is counter-intuitive 
because it points the opposite of what you'd expect:

* Since [UE 5.6 the axes changed] and now have in the "Top View" +X to the left, and +Y down

![Top view axes](images/top-down-axes.png)

  * The options for dealing with the confusion is to:
    * Not use ortho camera
      * Use perspective and try to position the camera
      * bad, inconvenient, have to orient the perspective view often
    * Use top and ortho but set the sprites and camera to match
      * also bad, have to rotate sprites every time

My current approach is the latter, I set the view to "Top", and it means for every sprite you place in 
your scene you need to rotate them like this:

![Axes changes for every sprite](images/axes-changes.png)

* Use the folder `MyAdventure/Textures/...` and put all the graphics in there
* Unreal will import them automatically but Paper2D requires some extra setup
    - right click and Sprite Actions > Apply Paper2D Texture Settings
    - ensure textures are set to the `TranslucentUnlit` sprite material
* From each texture right-click and Sprite Action > Create Sprite
    - drag the resulting sprite into a new `Assets/Sprites` folder
* Create Flipbooks from the sprites 
* Remember if you are using "Top" view to rotate as above so you can see

[UE 5.6 the axes changed]: https://forums.unrealengine.com/t/the-top-view-axis-has-changed-in-version-5-6-what-was-the-reasoning-behind-this-change/2552694/7

## Create the C++ Class 

* Tools > New C++ Class - Choose Pawn and name it `PlayerCharacter`
* Refresh the project & close Unreal
    - Can also use the `regen-unreal.sh` [script]
* Open `ProjName.code-workspace` which will launch VSCode
* Fill PlayerCharacter.{h,cpp} out per the code here - in VSCode
* Compile the code via **cmd-shift-B** > DesertRacerEditor Mac Development Build
* Open up Unreal Editor again via `ProjName.uproject`

[script]: https://gist.github.com/sarah-j-smith/cc4f2294de028c8be86c3255dfa52e17

## Blueprints
* Create a Blueprint via
    - Right click in a new folder `Content/Blueprints` and Create Blueprint Class
    - Search for `PlayerCharacter` - and name it `Blueprint_PlayerCharacter`
* Set the `Translucency Sort Priority` for the sprite so that it renders in order
    - Can set backgrounds and environment sprites to say `-100` up to `-10`
    - Can set the player to `0` 

## Create Inputs
* Create a `All/Content/Inputs` folder
* Right click and create an **Input Action** and an **Input Mapping Context**
    - set the input action to have a Vector 2D - this gives it 4 axes
    - set the input mapping context as per below
    - plug these into the slots in the blueprint created by the `UPROPERTY(EditAnywhere, BlueprintReadOnly)`

![input-context](input-context.png)

## Camera
* For 2D - want to set camera to Orthographic
* Use the spring arm to set the camera position via rotating it 90 degrees
* Turn the rotation to Global so it won't move with the rotation of the car
* Set the camera width to allow a decent view 

## Game Mode
* Create a new C++ Class for this based off the Game Mode base class
* Create a blueprint `BP_MainGameMode` based off of it and:
    - Edit > Project Settings > Maps & Modes
    - Default game mode: `BP_MainGameMode`
    - Default pawn class: `BP_PlayerCharacter`

## Obstacles
* Can create a single `Obstacle` class and 
    - Create a `BP_TyreObstacle` and a `BP_FenceObstacle` from it
        - just changing the sprite and collision capsule
        - turn off the collision preset - `NoCollision` for the sprite
        - the capsule will be `DynamicAll`

# Music

Background music can be launched off the Game Instance, a class instance
that persists between level changes. But the Start Game event cannot
be used for this - it has to be triggered from the Game Mode when the 
game starts.

## Create a Game Instance

* Right click in Blueprints folder and create a new Blueprint
* Base it off the Game Instance class (not the Platform Game Instance)
    - Call it `BP_MyGameInstance`
* Edit > Project Settings > Maps & Modes > Game Instance
    - Select `BP_MyGameInstance`

## Add the Logic for the Music to Start

* Inside `BP_MyGameInstance`

![screenshot of blueprint](./BP_MyGameInstance.png)

* Inside `BP_MyGameMode`

![screenshot of blueprint](./BP_MyGameMode.png)

## Use PaperZD for FSM Animation - Basic

* Extended state machine based sprite animation
* Start by creating an AnimationSource alongside the character blueprint
* Create animation sequences from flipbooks inside the asset browser
* Add any sound effects in the timeline notify dopesheet
* Create an Animation Blueprint for the character eg `AnimBP_CharacterName`
    * Right click next to the character blue print, choose PaperZD > AnimBP

![AnimBP creation](./create-anim-bp.png)

* Ensure in the character Blueprint eg `BP_CharacterName` the animation component"
    * is `ComponentClass`: `PaperZDAnimationComponent`
    * `PaperZD Anim Instance class`: `AnimBP_CharacterName`
* In `AnimBP_CharacterName` nav top left should have `Event Graph` and `Anim Graph`
* In the Event graph:
    * Use GetOwningActor cast to its class, then promote to variable eg `CharacterName`
    * Hook that to the event on init 
* In the Anim graph:
    * Create a state machine hooked output to the Output Animation
    * Create states for each animation to play 
    * Create the rules for the character to switch from state to state