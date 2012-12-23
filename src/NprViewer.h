//=============================================================================
//                                                                            
//   Exercise code for the lecture
//                                                                            
//=============================================================================
//=============================================================================
//
//  CLASS MeshViewer
//
//=============================================================================


#ifndef MESH_VIEWER_HH
#define MESH_VIEWER_HH


//== INCLUDES =================================================================
#include <string>
#include <vector>

#include "gl/TrackballViewer.h"
#include "gl/shader.h"
#include "gl/Mesh3D.h"
#include "gl/Light3D.h"
#include "gl/texture.h"
#include "gl/TextureArray.h"
#include "gl/fbo.h"


//== CLASS DEFINITION =========================================================


	      
/*
 NprViewer.
*/
class NprViewer : public TrackballViewer
{
public:
   
  NprViewer(const char* _title, int _width, int _height);
  
	
  void loadMesh(const std::string& filenameOBJ, const std::string& filenameMTL = std::string());
	
protected:

	// overloaded GUI function
  virtual void init();
	virtual void keyboard(int key, int x, int y);
virtual void reshape(int w, int h); 
	
  virtual void draw_scene(DrawMode _draw_mode);

private:
	void drawCartoon(unsigned int vertexIndex);
	void drawDepth();
	void drawEdge();
	void shakeEdge();
	void blendCartoonAndEdge();
	void drawTriangleByTriangle(unsigned int vertexIndex);
	void NormalPosAndUV(unsigned int x, Vector3 normal, Vector3 curvature, Vector2 center);
	Vector2 projectVertex(Vector3 vertex, const Matrix4& transScreenCoor);
	void updateMeshUV();
	void drawWholeMesh();

	void renderFullScreenQuad();
	
	
protected:
	
	// frame buffer object for render2texture
	FrameBufferObject m_fbo;
	
	// mesh object
	Mesh3D m_mesh;
	
	// cartoon shader
	Shader m_cartoonShader;
	
	// depth shader
	Shader m_depthShader;
	
	// shaking shader
	Shader m_shakingShader;

	// edge shader
	Shader m_edgeShader;

	// blending shader
	Shader m_blendingShader;
	
	// cartoon shading array texture (stroke textures)
	TextureArray m_cartoonShadingTexture;
	
	//Multipass rendering textures
	Texture m_firstPassTexture;
	Texture m_sndPassTexture;
	Texture m_thirdPassTexture;

	// cartoon output texture
	//Texture m_cartoonOutputTexture;
	
	// depth texture
	Texture m_depthTexture;

	// edge texture
	Texture m_edgeTexture;

	// shake texture
	Texture m_shakeEdgeTexture;

	// Paper texture
	Texture m_paperTexture;
	
	
};


//=============================================================================
#endif // MESH_VIEWER_HH defined
//=============================================================================
