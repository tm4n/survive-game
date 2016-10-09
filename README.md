# Survive!

Survive! is an online team-based shooter. Survive round after round of attacks by scary monsters.

Download the game at http://tman.bplaced.net/blog/

TODO: Add screenshots

# Source code

I'm releasing the source code after 3 years of development. I never planned to do this, so the code is quite messy.
There are some comments to help you understand. It's running on a custom developed OpenGL 3.0/ES2.0 engine with an extremely simple renderer,
making this a good project for beginners to look for clues on how to do basic shader based rendering.

I will update this file with a short overview of the folders and main files.
Due to copyright reasons, I did not include the models in this release. Some of them are bought with proprietary licenses,
some have been made by helpful community members. If you are truely interested in continuing development, contact me and maybe
we can work out how to properly licence all files. I plan to further develop the game, but time is short so I can't make any promises.

# Building the game

The game runs on Windows, Linux and the Ouya console (Android).
Check the "libs" folder for instructions on how to prepare the used libraries SDL (+mixer,ttf), glm and enet.

Use the Visual Studio 2013 build files for Windows XP and upwards.
Linux build is currently done with a Code::Blocks project file. Terrible, I know, I will add makefiles.
Android is done with ndkbuild and an Eclipse project.
