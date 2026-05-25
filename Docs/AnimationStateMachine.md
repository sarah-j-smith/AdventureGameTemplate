# Animation State Machine for Adventure Game

From the end of Lesser Dog's tutorial, I wanted to change and instead of using enums for the player animation states, use the PaperZD state machine implementation that I learned from the Game Dev TV course, as per the Crusty Pirate game.

PaperZD is a free plugin for Unreal Engine and seems stable, as well as full-featured. It has great documentation too:

* [PaperZD Documentation](https://www.criticalfailure-studio.com/paperzd-documentation/)

PaperZD multi-direction is a huge area and its out of scope for this how-to, so please refer to Paper ZD's documentation or the YouTube
tutorials linked below for more details, if needed.

## 1. Check the Paper ZD Character

* Check that the PaperZD PlugIn is enabled in Edit > Plugins
* Navigate to our `BP_AdventureCharacter` in the Content Drawer from the [How To Section 5.2]
  * If working from scratch, create a new C++ Class called `AdventureCharacter` inheriting from PaperZDCharacter
  * Compile the C++ project and then create a new Blueprint `BP_AdventureCharacter` based off that.
* Open `BP_AdventureCharacter` up in a new tab in the editor
* Confirm that its `Animation Component` class is a `PaperZDAnimationComponent`

[How To Section 5.2]: ./HowTo.md#52-setup-the-character-blueprint-and-flipbook

![PaperZDAnimationComponent](images/animation-component-is-zd.png)

## 2. Ensure Flipbook Assets are Correct

* You need separate left and right walk cycle sprites.
  * You can flip these in a paint program like Gimp if your sprite sheet doesn't have them
  * Do not attempt to use a -1 scale to do this, it won't work with PaperZD
  
![Left and right facing walk cycle](images/Walk-Sprite-Sheet.png)

Import these in the usual way, and create flipbooks. 

Note that you must have **the same number of frames (8) in each of the following flipbooks**:

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

## 3. Create an Animation Source and Blueprint

Next to the Player Character blueprint:

* Right click in the content browser and create a _Paper ZD_ > _Animation Source_
* For each of the flipbook sets click the "Add New" button in the assets list top left
* You'll at least have 
  * Walk - left, right, front & back
  * Idle - left, right, front & back
* Later you might have 
  * Interaction - left, right, front & back
* For each _Animation Sequence_ - name it using F2 - for Walk, Idle
  * Double-click the entry at the right (eg under Disk size) to activate that sequence
  * and set the 4 flipbooks in the details panel at the right
    * Use the `Multi-directional sequence`: true
    * then for each of the 4 quadrants set the matching flipbook
* These will appear in a folder next to the Animation Source

![PaperZD Anim Sources](./images/anim-sequence.png)

Select each of the four quadrants in the _Animation Data - 4 Directions_ pane and use the dropdown below it to select the appropriate flipbook for each direction.

Hold down the shift key and hover the mouse cursor over the brown quadrants to see how the various directions look when switched between.

If the walk animation looks weird in this view check that it has the same number of frames in its flip book for up & down, versus left and right.

* For more in depth tutorial content on this [J Barlow's PaperZD series](https://youtu.be/pwJecJZ0-_4?si=nfhf4QJZJawQhpUv&t=282)

## 4. Create the Animation Blueprint

Next to the Animation Source:

* Right click on the content browsesr and create a _Paper ZD_ > _Anim BP_
  * When prompted select the animation source just created 
  * Save it as `BP_AdventureAnimBP` 

* Connect the Player Character BP to the Animation Blueprint
  * Open up the editor tab with the `BP_AdventureCharacter`
  * Select the _Animation Component_ in the components list
  * Change the _Anim Instance Class_ drop-down in the details panel under the _Paper ZD_ section
      * set it to the `BP_AdventureAnimBP` just created

## 5. Set up the animations and direction in the AnimBP

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

![set up the directionality](images/animbp-directionality.png)

## 6. Test in Game

* With the above setup the character should now be able to walk around the scene

![Walk in scene](./images/walk.gif)

With any issues, recheck the settings in previous steps, and tutorials for more help with the AnimBP.

There's also a [issues](./Issues.md) page which has some information about nav mesh issues.