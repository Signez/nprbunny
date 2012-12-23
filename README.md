NPRBunny
========

Artistic non-photorealistic rendering project for EPFL course Introduction to Computer 
Graphics. Using OpenGL shaders, homemade textures and tweaks, it renders your favorite
mesh like a very talented artist would do with a pen.

These libraries are required:
 - FreeImage <http://freeimage.sourceforge.net/>
 - GLUT, GLEW and GL (from your graphic card driver)

This project should run on any graphic card that support GLSL 1.3 or more (OpenGL 3+),
on any OS. You may need to execute the program from the src/ directory, or move 
src/shaders in your working directory and moving data in the parent directory.

usage: nprbunny [path to rendered obj]

Created by Erwan Lucas, Joachim Joyaux and Stanislas Signoud, december 2012, based
on course material and lab framework from EPFL course Introduction to Computer Graphics).