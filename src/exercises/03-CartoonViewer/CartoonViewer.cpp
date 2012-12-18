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

	m_cartoonShadingTexture.create("../../../data/textures/brush_", ".png", 16);
	//m_cartoonShadingTexture.create("../../../data/textures/white-parchment-paper-texture.tga");
	m_paperTexture.create("../../../data/white-parchment-paper-texture.tga");
}



//-----------------------------------------------------------------------------


void
CartoonViewer::
reshape(int _w, int _h)
{
	TrackballViewer::reshape(_w,_h);

	// resize framebuffer and textures
	m_fbo.create(_w,_h, true);

	// Initialize textures
	m_firstPassTexture.create(_w,_h,GL_RGBA16,GL_RGB,GL_UNSIGNED_INT);
	m_sndPassTexture.create(_w,_h,GL_RGBA16,GL_RGB,GL_UNSIGNED_INT);
	m_thirdPassTexture.create(_w,_h,GL_RGBA16,GL_RGB,GL_UNSIGNED_INT);

	m_depthTexture.create(_w,_h,GL_RGBA16,GL_RGB,GL_FLOAT);
	m_edgeTexture.create(_w,_h,GL_RGB,GL_RGB,GL_UNSIGNED_INT);
	m_shakeEdgeTexture.create(_w,_h,GL_RGB,GL_RGB,GL_UNSIGNED_INT);

	// attach textures to frame buffer
	m_fbo.attachTexture(GL_COLOR_ATTACHMENT0_EXT, m_firstPassTexture.getID());
	m_fbo.attachTexture(GL_COLOR_ATTACHMENT1_EXT, m_sndPassTexture.getID());
	m_fbo.attachTexture(GL_COLOR_ATTACHMENT2_EXT, m_thirdPassTexture.getID());
	m_fbo.attachTexture(GL_COLOR_ATTACHMENT3_EXT, m_depthTexture.getID());
	m_fbo.attachTexture(GL_COLOR_ATTACHMENT4_EXT, m_edgeTexture.getID());
	m_fbo.attachTexture(GL_COLOR_ATTACHMENT5_EXT, m_shakeEdgeTexture.getID());

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
	drawCartoon(0);
	m_fbo.unbind();

	m_fbo.bind(GL_COLOR_ATTACHMENT1_EXT);
	drawCartoon(1);
	m_fbo.unbind();

	m_fbo.bind(GL_COLOR_ATTACHMENT2_EXT);
	drawCartoon(2);
	m_fbo.unbind();

	// draw depth image
	m_fbo.bind(GL_COLOR_ATTACHMENT3_EXT);
	drawDepth();
	m_fbo.unbind();

	// calculate edges on depth image
	m_fbo.bind(GL_COLOR_ATTACHMENT4_EXT);
	drawEdge();
	m_fbo.unbind();

	// shake the edges
	m_fbo.bind(GL_COLOR_ATTACHMENT5_EXT);
	shakeEdge();
	m_fbo.unbind();

	// blend edges and perform the three way blending
	blendCartoonAndEdge();

}

//-----------------------------------------------------------------------------
void 
CartoonViewer::
drawCartoon(unsigned int vertexIndex) {

	// clear screen
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	glShadeModel(GL_SMOOTH);
	glClearColor(1,1,1,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_cartoonShader.bind(); 

	// set parameters
	m_cartoonShader.setMatrix4x4Uniform("worldcamera", m_camera.getTransformation().Inverse());
	m_cartoonShader.setMatrix4x4Uniform("projection", m_camera.getProjectionMatrix());
	m_cartoonShader.setMatrix4x4Uniform("modelworld", m_mesh.getTransformation() );

	m_cartoonShadingTexture.setLayer(0);
	m_cartoonShadingTexture.bind();
	m_cartoonShader.setIntUniform("brushes", m_cartoonShadingTexture.getLayer());

	//draw the mesh Triangle by Triangle
	drawTriangleByTriangle(vertexIndex);

	//Draw the entire mesh
	//drawWholeMesh();

	//glDisable(GL_TEXTURE_2D);
	//glDisable(GL_CULL_FACE);

	m_cartoonShadingTexture.unbind();
	m_cartoonShader.unbind();
}

//--------------------------------------------------------
void CartoonViewer::drawTriangleByTriangle(unsigned int vertexIndex)
{
	//Update the UVcoordinates
	updateMeshUV();

	// For each part of the mesh
	for(unsigned int partMesh = 0; partMesh < m_mesh.getNumberOfParts(); partMesh++)
	{
		// For each triangle
		for(unsigned int triangle = 0; triangle < m_mesh.getNumberOfFaces(); triangle++)
		{
			unsigned int a, b, c;
			// Get each vertex of the considered triangle
			a = m_mesh.getFaceVertexIndex(triangle, 0, partMesh);
			b = m_mesh.getFaceVertexIndex(triangle, 1, partMesh);
			c = m_mesh.getFaceVertexIndex(triangle, 2, partMesh);

			Vector3 normal;
			Vector3 curvature; // We use the curvature in the same way
			switch (vertexIndex) {
			case 0:
				normal = m_mesh.getVertexNormal(a);
				curvature = (m_mesh.getVertexNormal(a) - m_mesh.getVertexNormal(b) ).cross( m_mesh.getVertexPosition(b) - m_mesh.getVertexPosition(a) );
				break;
			case 1:
				normal = m_mesh.getVertexNormal(b);
				curvature = (m_mesh.getVertexNormal(b) - m_mesh.getVertexNormal(c) ).cross( m_mesh.getVertexPosition(c) - m_mesh.getVertexPosition(b) );
				break;
			case 2:
				normal = m_mesh.getVertexNormal(c);
				curvature = (m_mesh.getVertexNormal(c) - m_mesh.getVertexNormal(a) ).cross( m_mesh.getVertexPosition(a) - m_mesh.getVertexPosition(c) );
				break;

			default:
				//Take the mean if fails
				normal = ( m_mesh.getVertexNormal(a) + m_mesh.getVertexNormal(a) + m_mesh.getVertexNormal(a) )/3.0;
				break;
			}

			Vector2 Coorcenter = ( m_mesh.getUVs()[a] + m_mesh.getUVs()[b] + m_mesh.getUVs()[c] )/3.0;

			//draw the triangle
			glBegin(GL_TRIANGLES);
			NormalPosAndUV(a, normal, curvature, Coorcenter);
			NormalPosAndUV(b, normal, curvature, Coorcenter);
			NormalPosAndUV(c, normal, curvature, Coorcenter);
			glEnd();
		}
	}
}

//--------------------------------------------------------

void CartoonViewer::NormalPosAndUV(unsigned int x, Vector3 normal, Vector3 curvature, Vector2 center)
{
	Vector3 A_normal = m_mesh.getVertexNormal(x);
	Vector3 A_position = m_mesh.getVertexPosition(x);
	Vector2 texCoord = m_mesh.getUVs()[x];



	//Modif pour normale et courbure uniforme dans triangle
	//glNormal3d(normal.x, normal.y, normal.z);
	// Conserver une interpolation
	A_normal = A_normal + normal / 2;
	glNormal3d(A_normal.x, A_normal.y, A_normal.z);
	glMultiTexCoord3d(GL_TEXTURE0, texCoord.x, texCoord.y, 0.0);
	glMultiTexCoord3d(GL_TEXTURE1, curvature.x, curvature.y, curvature.z);
	glMultiTexCoord2d(GL_TEXTURE2, center.x, center.y); //Finalement inutile mais � confirmer ...
	glVertex3d(A_position.x, A_position.y, A_position.z); 

}

//--------------------------------------------------------
void CartoonViewer::updateMeshUV()
{
	//clear old TexCoord
	m_mesh.m_vertexUV.clear();

	Matrix4 TransScreenCoor = (m_camera.getProjectionMatrix() * m_camera.getTransformation().Inverse() * m_mesh.getTransformation() );

	for( int j = 0 ; j < m_mesh.getNumberOfVertices(); j+=1)
	{
		//Calculate the UV coordinates on screen
		Vector2 t = projectVertex(m_mesh.getVertexPosition(j), TransScreenCoor);

		//add the uv-Coordinates
		m_mesh.m_vertexUV.push_back(t);
	}
}

//-----------------------------------------------------------------------------

Vector2 CartoonViewer::projectVertex(Vector3 vertex, const Matrix4 &transScreenCoor){
	//double *mesh_transform = ( m_camera.getTransformation().Inverse() * m_mesh.getTransformation() ).matToArray();
	//double *projectionMatrix = m_camera.getProjectionMatrix().matToArray();
	//GLint viewport[4];
	//glGetIntegerv( GL_VIEWPORT , viewport );

	/* set up some temporary floats for the values we �re going
	to need to store for texture coordinates */
	double t11 ;
	double t12 ;
	//GLdouble t13 ;

	/* This command projects the triangle �s coordinate points
	onto the window �s coordinate system */

	Vector3 ScreenCoor = transScreenCoor*vertex;

	/* seen as the window isn't square we need to remedy the
	stretching a little and so we divide the window
	coordinates by a factor of the total width and length .
	*/
	ScreenCoor = 0.5*( Vector3(1.0,1.0,1.0) + ScreenCoor );
	t11 = ScreenCoor.x;
	t12 = ScreenCoor.y;
	t11 = max(t11, 0.0);
	t12 = max(t12, 0.0);

	return Vector2(t11, t12);

}

//--------------------------------------------------------
void CartoonViewer::drawWholeMesh()
{
	//Update the UVcoordinates
	updateMeshUV();

	// setting vertex and Normal array for rendering
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer( 3, GL_DOUBLE, 0, m_mesh.getVertexPointer() );
	glNormalPointer( GL_DOUBLE, 0, m_mesh.getNormalPointer() );

	// texture coord 0: Using the calculates UV of the mesh on screen
	glClientActiveTextureARB(GL_TEXTURE0_ARB); 
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer( 2, GL_DOUBLE, 0, m_mesh.getUvTextureCoordPointer() );

	// texture coord 1: Using the curvature direction as a texture 
	glClientActiveTextureARB(GL_TEXTURE1_ARB);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(3, GL_FLOAT, 0, 0);

	// texture coord 2:
	glClientActiveTextureARB(GL_TEXTURE2_ARB);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(3, GL_FLOAT, 0, 0);


	for(unsigned int i = 0; i < m_mesh.getNumberOfParts(); i++)
	{
		//bool hasTexture = m_mesh.hasUvTextureCoord(); m_mesh.getMaterial(i).hasDiffuseTexture();
		glDrawElements( GL_TRIANGLES, m_mesh.getNumberOfFaces(i)*3, GL_UNSIGNED_INT, m_mesh.getVertexIndicesPointer(i) );
	}

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	// Disable multitex coordinates
	glClientActiveTextureARB(GL_TEXTURE0_ARB);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTextureARB(GL_TEXTURE1_ARB);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTextureARB(GL_TEXTURE2_ARB);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

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

	// Give the 3 stroke rendering to perform 3 way blending
	m_blendingShader.bind();
	m_firstPassTexture.setLayer(0);
	m_firstPassTexture.bind();
	m_blendingShader.setIntUniform("texture1",m_firstPassTexture.getLayer());

	m_blendingShader.bind();
	m_sndPassTexture.setLayer(1);
	m_sndPassTexture.bind();
	m_blendingShader.setIntUniform("texture2",m_sndPassTexture.getLayer());

	m_blendingShader.bind();
	m_thirdPassTexture.setLayer(2);
	m_thirdPassTexture.bind();
	m_blendingShader.setIntUniform("texture3",m_thirdPassTexture.getLayer());

	m_shakeEdgeTexture.setLayer(3);
	m_shakeEdgeTexture.bind();
	m_blendingShader.setIntUniform("ShakeEdge",m_shakeEdgeTexture.getLayer());

	m_paperTexture.setLayer(4);
	m_paperTexture.bind();
	m_blendingShader.setIntUniform("Paper",m_paperTexture.getLayer());


	// render a quad over full image
	renderFullScreenQuad();

	m_blendingShader.unbind();
}


//=============================================================================
