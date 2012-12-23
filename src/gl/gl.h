/**
 * OpenGL includes.
 *
 * Based on EPFL "Introduction to Computer Graphics" framework, cleaned
 * and improved by authors:
 *
 * Erwan Lucas,
 * Stanislas Signoud,
 * Joachim Joyaux.
 */

#ifndef GL_WRAPPERS_HH
#define GL_WRAPPERS_HH

//== INCLUDES =================================================================

//#include <iostream>


// Include files for OpenGL and GLUT have different paths under MacOS...

// Mac OS X
#ifdef __APPLE__
#  include <GLUT/glut.h>
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>

// Windows
#elif _WIN32
#  include <stdlib.h>
#  include <glew.h>
#  include <glut.h>
#  include <GL/gl.h>
#  include <GL/glu.h>

// Unix
#else
#  include <GL/glew.h>
#  include <GL/glext.h>
#  include <GL/glut.h>
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif

#define STRIP_CODE_

//=============================================================================
#endif // GL_WRAPPERS_HH defined
//=============================================================================
