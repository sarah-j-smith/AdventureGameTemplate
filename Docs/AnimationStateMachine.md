# Animation State Machine for Adventure Game

From the end of Lesser Dog's tutorial, I wanted to change and instead of using enums for the player animation states, use the PaperZD state machine implementation that I learned from the Game Dev TV course, as per the Crusty Pirate game.

PaperZD is a free plugin for Unreal Engine and seems stable, as well as full-featured. It has great documentation too:

* [PaperZD Documentation](https://www.criticalfailure-studio.com/paperzd-documentation/)

## Create a Paper ZD Character

* Check that the PaperZD PlugIn is enabled in Edit > Plugins
* Create a new C++ Class called `AdventureCharacter` inheriting from PaperZDCharacter
* Compile the C++ project and then create a new Blueprint `BP_AdventureCharacter` based off that.
* Open `BP_AdventureCharacter` up in a new tab in the editor
* Confirm that its `Animation Component` class is a `PaperZDAnimationComponent`

![PaperZDAnimationComponent](images/animation-component-is-zd.png)

## Create an Animation Source and Blueprint

Next to the Player Character blueprint:

* Right click in the content browser and create a _Paper ZD_ > _Animation Source_
* For each of the flipbooks click the "Add New" button in the assets list top left
    * And create a new _Animation Sequence_ - name it using F2
    * and set the flipbook in the details panel at the right
* These will appear in a folder next to the Animation Source

Next to the Animation Source:

* Right click on the content browsesr and create a _Paper ZD_ > _Anim BP_
* When prompted select the animation source just created 
* Save it as `BP_AdventureAnimBP` 

## Connect the Player Character BP to the Animation Blueprint

* Open up the editor tab with the `BP_AdventureCharacter`
* Select the _Animation Component_ in the components list
* Change the _Anim Instance Class_ drop-down in the details panel under the _Paper ZD_ section
    * set it to the `BP_AdventureAnimBP` just created

## Configure the PaperZD Character as Per Lesser Dog Paper Character

* Make the Capsule 4 half-height, and 4 radius
* Make the x-rotation -90
* Make the y-translation -28
* Setup the `BP_AdventureCharacter` event graph to use the _Simple Move to Location_ for now:

![Simple Move to Location connected to adventure char](images/simple-move-to-location.png)

# Confirm Working

At this point we should be back with things work as per Lesser Dog Tutorial end of Ep 1.

Run the game and confirm that the player character appears on the screen and you can move it around by clicking on the nav mesh. It won't be animated yet.

## Switch to 4-way character sprites

My next step is to ditch "TempGuy" and go to a 4-way multi-direction sprite.

There's two ways to handle flipping a character left to right when they walk or face left and then right during the game. Both depend on detecting the player character's velocity. Once you have the velocity you can:

1. Use an `FRotator` to rotate around the vertical access - in our case the Y
2. Use Paper ZD's directional sprites. 

The first way is quick when you only have one sprite animation, and its mentioned briefly in [this PaperZD video](https://www.youtube.com/watch?v=ogIyMrX_uws&t=1079s) as a good way to do it. Here's the C++ code to do it in the AAdventureCharacter::Tick() method:

```c++
	UCharacterMovementComponent *MovementComponent = GetCharacterMovement();
	FVector Velocity = MovementComponent->Velocity;
	FRotator CurrentRotation = Controller->GetControlRotation();
	if (Velocity.X > 0)
	{
		if (!CurrentRotation.Pitch == 0.0)
		{
			FRotator NewRotation = CurrentRotation;
			NewRotation.Pitch = 0.0f;
			Controller->SetControlRotation(NewRotation);
		}
	}
	else
	{
		if (!CurrentRotation.Pitch == 180.0)
		{
			FRotator NewRotation = CurrentRotation;
			NewRotation.Pitch = 180.0f;
			Controller->SetControlRotation(NewRotation);
		}
	}
```

But for more complex scenarios the second is more scalable. It also means that all the character movement state is handled by PaperZD instead of having to do some in code/blueprints and some in PaperZD State Machine.

The first thing needed to make this work is to have 4-way sprites for walk and for idle animations. If the sprite sheet doesn't have left-facing, and only supplies right-facing idle or walk assets, then use an art program to flip the sheet. This [template for a point and click character from Danger Goose](https://danger-goose.itch.io/point-and-click-adventure-game-sprite-template) shows what you need:

![Left and right facing walk cycle](images/Walk-Sprite-Sheet.png)

Import these in the usual way, and create flipbooks. Note that you must have **the same number of frames (8) in each of the following flipbooks**:

* Adv_Walk_Left_Flipbook
* Adv_Walk_Right_Flipbook
* Adv_Walk_Up_Flipbook
* Adv_Walk_Down_Flipbook

These ones also need to have the same number of frames:

* Adv_Idle_Left_Flipbook
* Adv_Idle_Right_Flipbook
* Adv_Idle_Up_Flipbook
* Adv_Idle_Down_Flipbook

So duplicate frames in the flipbooks to pad out as needed.

## Add in the Animation Sequences as Multi-directional

Now add two Animation Sequences to the Animation Source, editing the ones that were there, and set them both to _Multi-directional Sequence_ until it looks like this:

![Animation sequence multi-direction](images/anim-sequence.png)

Select each of the four quadrants in the _Animation Data - 4 Directions_ pane and use the dropdown below it to select the appropriate flipbook for each direction.

Hold down the shift key and hover the mouse cursor over the quadrants to see how the various directions look when switched between.

If the walk animation looks weird in this view check that it has the same number of frames in its flip book for up & down, versus left and right.

## Set up the animations and direction in the AnimBP

In the `BP_AdventureAnimBP` the hookup of all these assets needs to be done. See the graphs navigator panel at left in the first "save to variable" screenshot below for a tree view of the AnimBP once its complete.

* Event graph - save the Adventure Character to a variable

![save the Adventure Character to a variable](images/AdventureAnimBP_Event_Graph.png)

* Check the State Machine has the correct states and transitions

![State Machine correct states](images/AdventureAnimBP%20State%20Machine.png)

* In the Idle state set the idle animation

![Idle state set the idle animation](images/Play%20idle%20animation.png)

* In the Walk state set the walk animation

![Walk state set the walk animation](images/Play%20walk%20animation.png)

* In the Walk-to-Idle rule transition when the velocity is zero

![Walk-to-Idle rule transition](images/walk-to-idle-rule.png)

The Idle-to-Walk rule is similar but transitions when the velocity is greater than zero.

Finally, set up the directionality. See the PaperZD documentation for more help on this if its problematic.

![set up the directionality](images/set-directionality.png)

