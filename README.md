# S.W.A.T. 2D
A S.W.A.T. Tactical Shooter game where the player must eliminate targets, collect intel, and rescue the hostage.

# Controls
| Controls          | Description        |
|-------------------|--------------------|
| Q                 | Debug              |
| E                 | Use                |
| Num Row           | Dialog Response    |
| Left Click        | Shoot              |
| Left Click + Drag | Draw Path          |
| W                 | Precise Move Up    |
| A                 | Precise Move Left  |
| S                 | Precise Move Down  |
| D                 | Precise Move Right |

# Menu Demo
[![Menu](https://i.imgur.com/jhgXuJ7.gif)](https://i.imgur.com/jhgXuJ7.mp4)

# gameframework2d (GF2D)
a collection of utility functions designed to facilitate creating 2D games with SDL2
This project is specifically intended to function as an educational tool for my students taking 2D Game Programming.

Currently the project is in a WIP state, however it is functional with sample projects showcasing rudimentary scrolling shooters,
platformers, and adventure games.

The master branch is kept deliberately minimal to allow students to build out systems as they are learning.
Other branches have more complete system: Collisions, menus, input abstraction.

# Build Process

Before you can build the example code we are providing for you, you will need to obtain the libraries required
by the source code
 - SDL2
 - SDL2_image
 - SDL2_mixer
 - SDL2_ttf
There are additional sub modules that are needed for this project to work as well, but they can be pulled right from within the project.
Performable from the following steps from the root of the cloned git repository within a terminal.

Make sure you fetch submodules: `git submodule update --init --recursive`
Go into each submodule's src directory and type:
`make`
`make static`

Once each submodule has been made you can go into the base project src folder and simply type:
`make`

You should now have a `gf2d` binary within the root of your git repository. Executing this will start your game.
