/**
 * GlutViewer class implementations.
 *
 * Based on EPFL "Introduction to Computer Graphics" framework, cleaned
 * and improved by authors:
 *
 * Erwan Lucas,
 * Stanislas Signoud,
 * Joachim Joyaux.
 */

//== INCLUDES =================================================================

#include "GlutViewer.h"


//== IMPLEMENTATION ========================================================== 


std::map<int, GlutViewer*>  GlutViewer::windows__;


//-----------------------------------------------------------------------------


GlutViewer::
GlutViewer(const char* _title, int _width, int _height)
  : width_(_width), height_(_height), draw_mode_(SOLID_SMOOTH), fullscreen_(false),
    bak_left_(0), bak_top_(0), bak_width_(0), bak_height_(0), menuID_(0)
{
  // create window
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_ALPHA | GLUT_MULTISAMPLE );
  //glutInitDisplayString("rgb double alpha depth>=16 samples>=16");
  glutInitWindowSize(_width, _height);
  windowID_ = glutCreateWindow(_title);
  windows__[windowID_] = this;


  // register callbacks
  glutDisplayFunc(display__);
  glutKeyboardFunc(keyboard__);
  glutSpecialFunc(special__);
  glutMouseFunc(mouse__);
  glutMotionFunc(motion__);
  glutPassiveMotionFunc(passivemotion__);
  glutReshapeFunc(reshape__); 
  glutVisibilityFunc(visibility__);
  glutIdleFunc(idle__);
  
  //Init glew for windows
  //#ifdef _WIN32
    glewInit();
  //#endif
}
  

//-----------------------------------------------------------------------------


GlutViewer::
~GlutViewer()
{
  glutDestroyWindow(windowID_);
  glutDestroyMenu(menuID_);
}


//-----------------------------------------------------------------------------


GlutViewer* GlutViewer::current_window() {
  return windows__[glutGetWindow()];
}

void GlutViewer::display__(void) {
  current_window()->display();
}

void GlutViewer::idle__(void) {
  current_window()->idle();
}

void GlutViewer::keyboard__(unsigned char key, int x, int y) {
  current_window()->keyboard((int)key, x, y);
}

void GlutViewer::motion__(int x, int y) {
  current_window()->motion(x, y);
}

void GlutViewer::mouse__(int button, int state, int x, int y) {
  current_window()->mouse(button, state, x, y);
}

void GlutViewer::passivemotion__(int x, int y) {
  current_window()->passivemotion(x, y);
}

void GlutViewer::reshape__(int w, int h) {
  current_window()->reshape(w, h);
}

void GlutViewer::special__(int key, int x, int y) {
  current_window()->special(key, x, y);
}

void GlutViewer::visibility__(int visible) {
  current_window()->visibility(visible);
}


//----------------------------------------------------------------------------


void GlutViewer::keyboard(int key, int x, int y) 
{
  switch (key)
  {
    case 'f': 
    {
      if (!fullscreen_) 
      {
				bak_left_   = glutGet(GLUT_WINDOW_X);
				bak_top_    = glutGet(GLUT_WINDOW_Y);
				bak_width_  = glutGet(GLUT_WINDOW_WIDTH);
				bak_height_ = glutGet(GLUT_WINDOW_HEIGHT);
				glutFullScreen();
				fullscreen_ = true;
      }
      else
      {
				glutReshapeWindow(bak_width_, bak_height_);
				glutPositionWindow(bak_left_, bak_top_);
				fullscreen_ = false;
      }
      break;
    }

    // ESC
	case 27:  
    {
      exit(0); 
      break;
    }
	}
}


//=============================================================================
