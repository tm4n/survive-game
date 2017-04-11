The build system expects the following folders here containing the libraries:

 - enet
 - glm
 - SDL2
 - SDL2_mixer
 - SDL2_ttf-2.0.12
 
 glm is a header only library. enet should be statically build. The SDL libraries are used as dynamic libs in Windows,
 and statically linked on the other platforms.
