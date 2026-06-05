# Intalling the Plugin

At present the plugin is:

* C++ source code
* Blueprints 
* Assets and content

It is not available on the FAB marketplace, and does not come as a binary installable plugin.

To install it requires:

* A [C++ compile environment on your local machine]
* A current 5.6 or better version of Unreal Engine
* Familiarity with file operations on your OS (Windows, Mac or Linux)

More help:
* [Additional Unreal Engine help articles for setting up C++]

[C++ compile environment on your local machine]: https://dev.epicgames.com/documentation/unreal-engine/unreal-engine-cpp-quick-start
[Additional Unreal Engine help articles for setting up C++]: https://dev.epicgames.com/documentation/unreal-engine/setting-up-visual-studio-code-for-unreal-engine#installingthecompilertoolset


## Install into a Game

* Either
  * Create a new Unreal Engine Game with C++ and Blueprints - OR -
  * [Convert an existing Unreal Engine Blueprint game to C++]
* Then
  * Download the zip file of the plugin from the [Github release page]
  * Ensure that your game has a top-level folder called `Plugins`

  * 1. Unzip the file locally

![Unzip the file locally](./images/open-zip-file.png)

* 2. Drag the file into the project's `Plugins` folder

![Drag to plugins](./images/drag-to-game-plugins.png)

* Ensure the plugin is enabled in _Edit > Plugins_

![Plugin enabled](./images/Installed-Adventure-Plugin.png)

* Add `AdventureTools` as a dependency in your game's Build.cs
* Build your project as normal

* Additional help articles
  * [How to manually install an Unreal Engine Plugin]

[How to manually install an Unreal Engine Plugin]: https://unreal-garden.com/tutorials/how-to-install-plugin/

[Convert an existing Unreal Engine Blueprint game to C++]: https://allarsblog.com/2015/11/05/converting-bp-project-to-cpp/

[Github release page]: https://github.com/sarah-j-smith/AdventureGameTemplate/releases

## Install into your Engine

_Totally untested_

* Download the zip file of the plugin from the [Github release page]
* Ensure that your engine has a top-level folder called `Plugins`
* Unzip the file locally and drag the `AdventureTools` folder into your `Plugins` folder
  * Epic docs on [Location of `Plugins` folder on Windows and Mac]

[Location of `Plugins` folder on Windows and Mac]: https://dev.epicgames.com/documentation/unreal-engine/working-with-plugins-in-unreal-engine#plugin-installation-locations