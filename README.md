# GO:SX Internal LITE

A basic internal Hack for macOS
___

### Features

- Basic Aimbot (w/ RCS, Smooth, Silent Aim)
- Chams (Textured & Flat)
- Box ESP (Healthbar & Name)
- Smoke ESP 
- Triggerbot (with adjustable delay)
- Bunnyhop
- SkinChanger
- Show Ranks on Scoreboard
- No/Reduce Flash
- Clantag Changer
- Always RCS 

___

### Requirements
- XCode 8 or later
- macOS Sierra (for XCode 8 and C++14)
- C++14 Compiler

___

### How to run

Build the source or Download the latest release from [gosx.one](http://www.gosx.one)

-

> If you downloaded the lastest release:
> 
> - Extract all files to a folder of your choice
> - In Terminal navigate to that folder
> - Run CS:GO
> - execute `sudo ./gosx` (type in your admin password when ask)
> - A MessageBox apears so the Hack is injected

-

> If you build on your own:
> 
> - Navigate to product folder with terminal (in XCode right click the gosx.dylib and the `Show in Finder`)
> - Run CS:GO
> - execute `sudo ./gosx` (type in your admin password when ask)
> - A MessageBox apears so the Hack is injected

-

When ingame (connected to server) press `Fn` + `Backspace` (or simply DEL on full featured Keyboard) to open the ingame menu

___

### How to Build

- Open GO:SX Lite Workspace,
- Fix the Settings (Build path, deployment target and architecture),
- Build the dylib, osxinj and bootstrap.dylib (all files incl.)
- Copy the Files `settings.ini`, `menu.ini` and `weapons.ini` from Engine folder to the `gosx.dylib` folder.

___

### Skin Changer

To edit your Weapon Config please refer to [http://www.gosx.one/skin-changer-config/](http://www.gosx.one/skin-changer-config/).
There you can upload your current weapons.ini to edit it or create a new one from scratch.

___

### Questions

Feel free to join me on discord:
[https://discord.gg/46GYf8K](https://discord.gg/46GYf8K)

___

### Contributions
Feel free to create pull requests or feature branches if you want to put something into that Project.
But do it the right way. Every contribution that breaks the project will be reverted uncommented. Also you will loose your contributor state to this project.

**You have been warned.**

___

### Screeshots

![](https://image.ibb.co/jT588v/Bildschirmfoto_2017_03_28_um_18_39_41.png)

![](https://image.ibb.co/eT0VMF/Bildschirmfoto_2017_03_28_um_18_40_20.png)

![](https://image.ibb.co/hBp1Tv/Bildschirmfoto_2017_03_28_um_18_48_14.png)

___

### License

This project is covered by the Apache 2.0 License. Carefully read LICENSE file to cover all laws of this License.

___

### Credits

- ViKinG (for finding a way to get Internals work on macOS)
- MarcHC (for his Simple Base i learned the most from)
- Aimtux Project (for Indexes)

___

### pssst*

Ignore the FPS Counter on the left top site. It is that low because I tabbed out of the game.