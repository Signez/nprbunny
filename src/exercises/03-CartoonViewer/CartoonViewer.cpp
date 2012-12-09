//=============================================================================
//                                                                            
//   Exercise code for the lecture
//                                                                            
//=============================================================================
//=============================================================================
//
//  CLASS CartoonViewer - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "CartoonViewer.h"
#include "../../utils/Mesh3DReader.h"

//== IMPLEMENTATION ========================================================== 


CartoonViewer::
CartoonViewer(const char* _title, int _width, int _height)
: TrackballViewer(_title, _width, _height)
{
  init();
}


//-----------------------------------------------------------------------------


void 
CartoonViewer::
init()
{
	// initialize parent
	TrackballViewer::init();

	// set camera to look at world coordinate center
	set_scene_pos(Vector3(0.0, 0.0, 0.0), 2.0);

	// load shaders
	m_cartoonShader.create("cartoon.vs", "cartoon.fs");
	m_depthShader.create("depth.vs", "depth.fs");
	m_edgeShader.create("edge.vs", "edge.fs");
	m_shakingShader.create("shaking.vs", "shaking.fs");
	m_blendingShader.create("blending.vs","blending.fs");

	// setup 1D color texture with 4 colors
	/*float  tex[12] =
	{
		0.2, 0.2, 0.2,
		0.4, 0.4, 0.4,
		0.6, 0.6, 0.6,
		0.8, 0.8, 0.8
	};*/

	//m_cartoonShadingTexture.create(4, 1, GL_RGB, GL_RGB, GL_FLOAT, tex, GL_NEAREST);
	m_cartoonShadingTexture.create("../../../data/Tex_Strokes.tga");
}



//-----------------------------------------------------------------------------


void
CartoonViewer::
reshape(int _w, int _h)
{
	TrackballViewer::reshape(_w,_h);
	
	// resize framebuffer and textures
	m_fbo.create(_w,_h, true);
	
	m_cartoonOutputTexture.create(_w,_h,GL_RGBA16,GL_RGB,GL_UNSIGNED_INT);
	// try GL_RGB4, GL_RGB8, GL_RGB10, GL_RGB16 to see effect of edge precision
	m_depthTexture.create(_w,_h,GL_RGBA16,GL_RGB,GL_FLOAT);
	m_edgeTexture.create(_w,_h,GL_RGB,GL_RGB,GL_UNSIGNED_INT);
	m_shakeEdgeTexture.create(_w,_h,GL_RGB,GL_RGB,GL_UNSIGNED_INT);
	
	// attach textures to frame buffer
	m_fbo.attachTexture(GL_COLOR_ATTACHMENT0_EXT, m_cartoonOutputTexture.getID());
	m_fbo.attachTexture(GL_COLOR_ATTACHMENT1_EXT, m_depthTexture.getID());
	m_fbo.attachTexture(GL_COLOR_ATTACHMENT2_EXT, m_edgeTexture.getID());
	m_fbo.attachTexture(GL_COLOR_ATTACHMENT3_EXT, m_shakeEdgeTexture.getID());
	
}


//-----------------------------------------------------------------------------

void
CartoonViewer::
loadMesh(const std::string& filenameOBJ, const std::string& filenameMTL)
{
	// load mesh from obj
	Mesh3DReader::read( filenameOBJ, m_mesh, filenameMTL);

	// calculate normals
	m_mesh.calculateVertexNormals();
	//m_mesh.genericUV();
	
	// get bounding box & reset scene camera accordingly
	Vector3 bbmin, bbmax;
	m_mesh.calculateBoundingBox(bbmin, bbmax);
	
	double radius = 0.5*(bbmin - bbmax).length();
	Vector3 center = 0.5*(bbmin + bbmax);
	
	set_scene_pos(center, radius);


}



//-----------------------------------------------------------------------------


void
CartoonViewer::
keyboard(int key, int x, int y)
{
	switch (key)
	{			
		case 'h':
			printf("Help:\n");
			printf("'h'\t-\thelp\n");
			break;
		default:
			TrackballViewer::special(key, x, y);
			break;
	}
	
	glutPostRedisplay();
}

//-----------------------------------------------------------------------------


void 
CartoonViewer::
draw_scene(DrawMode _draw_mode)
{
	// draw cartoon shading
	m_fbo.bind(GL_COLOR_ATTACHMENT0_EXT);
	drawCartoon();
	m_fbo.unbind();
	
	// draw depth image
	m_fbo.bind(GL_COLOR_ATTACHMENT1_EXT);
	drawDepth();
	m_fbo.unbind();
	
	// calculate edges on depth image
	m_fbo.bind(GL_COLOR_ATTACHMENT2_EXT);
	drawEdge();
	m_fbo.unbind();

	// shake the edges
	m_fbo.bind(GL_COLOR_ATTACHMENT3_EXT);
	shakeEdge();
	m_fbo.unbind();
	
	// blend edges and cartoon shading
	blendCartoonAndEdge();
	
}

//--------------------------------------------------------
void CartoonViewer::NormalPosAndUV(unsigned int x, float xUV, float yUV)
{
	Vector3 A_normal = m_mesh.getVertexNormal(x);
	Vector3 A_position = m_mesh.getVertexPosition(x);
	glVertex3d(A_position.x, A_position.y, A_position.z); //v1
	glNormal3d(A_normal.x, A_normal.y, A_normal.z);
	glTexCoord2d(xUV,yUV);
}

//-----------------------------------------------------------------------------

void 
CartoonViewer::
drawCartoon() {
	
	// clear screen
	glEnable(GL_DEPTH_TEST);
	glClearColor(1,1,1,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	m_cartoonShader.bind(); 
	
	// set parameters
	m_cartoonShader.setMatrix4x4Uniform("worldcamera", m_camera.getTransformation().Inverse());
	m_cartoonShader.setMatrix4x4Uniform("projection", m_camera.getProjectionMatrix());
	m_cartoonShader.setMatrix4x4Uniform("modelworld", m_mesh.getTransformation() );
	
	/*glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	//if(m_mesh.hasUvTextureCoord())
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);*/

	
	
	m_cartoonShadingTexture.setLayer(0);
	m_cartoonShadingTexture.bind();
	m_cartoonShader.setIntUniform("texture", m_cartoonShadingTexture.getLayer());
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	
	/*glVertexPointer( 3, GL_DOUBLE, 0, m_mesh.getVertexPointer() );
	glNormalPointer( GL_DOUBLE, 0, m_mesh.getNormalPointer() );
	//if(m_mesh.hasUvTextureCoord())
		glTexCoordPointer( 2, GL_DOUBLE, 0, m_mesh.getUvTextureCoordPointer() );

	for(unsigned int i = 0; i < m_mesh.getNumberOfParts(); i++)
	{
		bool hasTexture = m_mesh.hasUvTextureCoord();// m_mesh.getMaterial(i).hasDiffuseTexture();
		glDrawElements( GL_TRIANGLES, m_mesh.getNumberOfFaces(i)*3, GL_UNSIGNED_INT, m_mesh.getVertexIndicesPointer(i) );
	}
	
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	//if(m_mesh.hasUvTextureCoord())
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);*/


	// I'AM DOING STH WRONG HERE. MY BAD GUYS

	// For each part of the mesh
	for(unsigned int partMesh = 0; partMesh < m_mesh.getNumberOfParts(); partMesh++)
	{
		// For each triangle
		for(unsigned int triangle = 0; triangle < m_mesh.getNumberOfFaces(); triangle++)
		{
			unsigned int x, y, z;
			// Get each vertex of the considered triangle
			x = m_mesh.getFaceVertexIndex(triangle, 0, partMesh);
			y = m_mesh.getFaceVertexIndex(triangle, 1, partMesh);
			z = m_mesh.getFaceVertexIndex(triangle, 2, partMesh);

			glBegin(GL_TRIANGLES);
				NormalPosAndUV(x ,0.0, 0.0);
				NormalPosAndUV(y , 0, 0.1);
				NormalPosAndUV(z , 0.1, 0.1);
			glEnd();
		}
	}

	glDisable(GL_TEXTURE_2D);

	m_cartoonShadingTexture.unbind();
	m_cartoonShader.unbind();
}



//-----------------------------------------------------------------------------
void 
CartoonViewer::
drawDepth() {
	
	// clear screen
	glEnable(GL_DEPTH_TEST);
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	m_depthShader.bind(); 
	
	// set parameters
	m_depthShader.setMatrix4x4Uniform("worldcamera", m_camera.getTransformation().Inverse());
	m_depthShader.setMatrix4x4Uniform("projection", m_camera.getProjectionMatrix());
	m_depthShader.setMatrix4x4Uniform("modelworld", m_mesh.getTransformation() );
	m_depthShader.setMatrix3x3Uniform("worldcameraNormal", m_camera.getTransformation().Transpose());
	m_depthShader.setMatrix3x3Uniform("modelworldNormal", m_mesh.getTransformation().Inverse().Transpose());

	m_depthShader.setFloatUniform("near",m_camera.getNearPlane());
	m_depthShader.setFloatUniform("far",m_camera.getFarPlane());
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	
	glVertexPointer( 3, GL_DOUBLE, 0, m_mesh.getVertexPointer() );
	glNormalPointer( GL_DOUBLE, 0, m_mesh.getNormalPointer() );
	
	for(unsigned int i = 0; i < m_mesh.getNumberOfParts(); i++)
	{
		glDrawElements( GL_TRIANGLES, m_mesh.getNumberOfFaces(i)*3, GL_UNSIGNED_INT, m_mesh.getVertexIndicesPointer(i) );
	}

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	m_depthShader.unbind();
	
}

//-----------------------------------------------------------------------------

void 
CartoonViewer::
renderFullScreenQuad()
{
	// render full screen quad (note that vertex coordinates are already in opengl coordinates, so no transformation required!)
	glBegin(GL_QUADS);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 0.0f);
	glVertex2f(-1.0f, -1.0f);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1.0f, 0.0f);
	glVertex2f(1.0f, -1.0f);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1.0f, 1.0f);
	glVertex2f(1.0f, 1.0f);
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glEnd();
}


//-----------------------------------------------------------------------------
void 
CartoonViewer::
drawEdge() {
	
	// clear screen
	glDisable(GL_DEPTH_TEST);
	
	m_edgeShader.bind(); 
	//m_depthTexture.setLayer(0);
	m_depthTexture.bind();
	m_edgeShader.setIntUniform("texture", m_depthTexture.getLayer());
	m_edgeShader.setFloatUniform("dx",1.0/width_);
	m_edgeShader.setFloatUniform("dy",1.0/height_);
	
	// render a quad over full image
	renderFullScreenQuad();
	
	m_edgeShader.unbind();
	
}


//-----------------------------------------------------------------------------
void 
CartoonViewer::
shakeEdge() {
	
	// clear screen
	glDisable(GL_DEPTH_TEST);
	
	m_shakingShader.bind(); 
	//m_depthTexture.setLayer(0);
	m_edgeTexture.bind();
	m_shakingShader.setIntUniform("texture", m_edgeTexture.getLayer());
	
	// render a quad over full image
	renderFullScreenQuad();
	
	m_shakingShader.unbind();
	
}

//-----------------------------------------------------------------------------
void 
CartoonViewer::
blendCartoonAndEdge() {
	
	// clear screen
	glDisable(GL_DEPTH_TEST);
	
	m_blendingShader.bind();
	m_cartoonOutputTexture.setLayer(0);
	m_cartoonOutputTexture.bind();
	m_blendingShader.setIntUniform("texture1",m_cartoonOutputTexture.getLayer());
	/*m_edgeTexture.setLayer(1);
	m_edgeTexture.bind();
	m_blendingShader.setIntUniform("texture2",m_edgeTexture.getLayer());*/

	m_shakeEdgeTexture.setLayer(1);
	m_shakeEdgeTexture.bind();
	m_blendingShader.setIntUniform("texture2",m_shakeEdgeTexture.getLayer());
	
	
	// render a quad over full image
	renderFullScreenQuad();
	
	m_blendingShader.unbind();
}


//=============================================================================
