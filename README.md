# 2D Point and Click Adventure Game Plugin

* Current status: working - still much to do

_Why dis?_

In 2025 a C++ port was started of a blueprint [Point and Click 2D Adventure Game tutorial]. The port also moved
to using PaperZD, Common UI and EnhancedInput. The goal was to be able to make a new game, that was not just
the tutorial. [That project was archived] because the C++ port of the tutorial code was:

* Painful to make a new game from
* Content and features were mixed up

_Plugin architecture to the rescue?_

The only way to fix this and make it re-usable was to ensure [game features are encapsulated in plugins]. That way
any new game can add the plugin and get all the features, and build the game on top. Anything useful could 
still be upstreamed back into the plugin.

There is an [issues page](./Docs/Issues.md) to track what I need to fix next. I'll post some updates there as things get fixed.

[That project was archived]: https://github.com/sarah-j-smith/AdventureGame
[game features are encapsulated in plugins]: https://medium.com/@sarah.hyperdense/ue5-project-folder-structure-and-organization-best-practices-b9e487c330a3

## What is this?

Its a plugin for Unreal Engine that includes functionality useful for making Point and
Click adventure games in the style of Lucas Arts or Sierra Online. [Gabriel Knight 1] is the main
inspiration (640 x 480) as opposed to [Day of the Tentacle] (320 x 200).

* Character movement
* Hotspots
* Inventory
* Dialogue

This is a free plugin that is built on the blueprint based 
[Point and Click 2D Adventure Game tutorial]. The assets included in the plugin
come from there and are _not_ free. 

You should use your own art with any game you ship. I plan to add my stone chiselled GK1 style
buttons here for free use at some point.

[Gabriel Knight 1]: https://www.reddit.com/r/dosgaming/comments/1jrfx44/gabriel_knight_sins_of_the_fathers_sierra_online/
[Day of the Tentacle]: https://www.mobygames.com/game/719/maniac-mansion-day-of-the-tentacle/screenshots/

## How to create a game

### [Start here]

[Start here]: ./Docs/HowTo.md

# Developer Notes

My Dev Environment:

* Mac M3
* Rider
* Unreal 5.5
* MacOS / Windows 10

![Screenshot from Ch 1](Docs/images/adventure-clip.gif)

* Screenshot of state at end of Episode 5.

I implemented the main game logic in Unreal C++. At the present time I'm ready to move to using
the framework to make an original game, and I'm not following the tutorial from this point.

Surface level it looks very much the same as Lesser Dog's [Point and Click 2D Adventure Game tutorial] on Youtube. See here for the changes:

* [Differences from Lesser Dog tut](./Docs/Differences.md)

[Point and Click 2D Adventure Game tutorial]: https://www.youtube.com/watch?v=sEy3c5JcLys&t=7s

# Learnings

* [PaperZD Animation State Machine](./Docs/AnimationStateMachine.md)
* [Managing the Commands](./Docs/CommandState.md)
* [Issues that I ran into](./Docs/Issues.md)
* [How to customise the screen resolution for other games](./Docs/ScreenAndCamera.md)
* [How to make this game from scratch](./Docs/)

# Credits

The contents of this repository are _not generally free_ to use. Please see the credits listed below for the files
which are **not governed** by this repository's license, the file [MIT NO-AI License].

## Fonts Update & Credit

* I'm using [Press Start 2 Play](https://www.zone38.net/font/) now
* [License text](./3rdParty/LICENSE.txt)

It's a full-unicode retro gaming font. As far as I can tell Cody is the original
author of this font.

## Sounds Credit

Door - Stone - Large - 001.wav by DWOBoyle -- https://freesound.org/s/474178/ -- License: Attribution 4.0

button 6.wav by bubaproducer -- https://freesound.org/s/107153/ -- License: Attribution 4.0

VS Button Click 04.mp3 by Vilkas_Sound -- https://freesound.org/s/707041/ -- License: Attribution 4.0

Looping Gentle Wind Ambience on an Open Desert Plain.wav by dhallcomposer -- https://freesound.org/s/697217/ -- License: Creative Commons 0

Button hover.wav by Fachii -- https://freesound.org/s/338229/ -- License: Creative Commons 0

## License Credit

[![Made by Human](https://madebyhuman.iamjarl.com/badges/made-white.svg)](https://madebyhuman.iamjarl.com)

* The [MIT NO-AI License] is from the [Non-AI Licenses Github] project

The [arguments for and against non-AI licenses] are well understood. No correspondence will be entered 
into on these licenses and their presence in this repository. If you are planning on using the software
here to make games with then you have no problems. 

[MIT NO-AI License]: LICENSE.md
[Non-AI Licenses Github]: https://github.com/non-ai-licenses/non-ai-licenses
[arguments for and against non-AI licenses]: https://news.ycombinator.com/item?id=46411275

## Broken by Design

None of this crap works right now.

![Under construction](./Docs/images/AtAthensAtrium4970construction.gif)

This project is basically me futzing around with stuff. It won't work, or if parts of it does that is
nothing short of a miracle. Expect nothing and you won't be dissapointed.

![Under construction](./Docs/images/AtAthens6321underconstruction_lemmings.gif)

_GeoCities under construction gifs deployed [from here] with much irony intended_

[from here]: http://www.textfiles.com/underconstruction/