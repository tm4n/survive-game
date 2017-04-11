The build system expects the following folders here containing the libraries:

 - enet
 - glm
 - SDL2
 - SDL2_mixer
 - SDL2_ttf
 
 glm is a header only library. enet should be statically build. The SDL libraries are used as dynamic libs in Windows,
 and statically linked on the other platforms.

On Windows:
Download the development version for VC of the SDL2 libraries. Use the normal source versions for enet and glm.
Extracted them into this folder and remove the version number from the folder name for all libs.
Then go in to SDL2[..] folders, go into the subfolder "include" and create a folder named SDL2.
Copy the contents from the include folder into the newly created SDL2-Folder. This is for linux compability. Ugly, but works.
You'll need to copy the dll files from the SDL projects into the folder where you want to start survive_cl_win32 after compilation.
For enet, open the enet.dsp visual studio project and build debug and release versions.