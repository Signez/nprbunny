/**
 * Simple texture class definition.
 *
 * Based on EPFL "Introduction to Computer Graphics" framework, cleaned
 * and improved by authors:
 *
 * Erwan Lucas,
 * Stanislas Signoud,
 * Joachim Joyaux.
 */
#ifndef __TEXTURE_H__
#define __TEXTURE_H__
#include <string>
#include "gl.h"

class Texture 
{
public:
	Texture();
	Texture(unsigned int _width, 
			unsigned int _height, 
			unsigned int _internalFormat, 
			unsigned int _format,
			unsigned int _type);
	~Texture();
	void create(unsigned int _width, 
				unsigned int _height, 
				unsigned int _internalFormat, 
				unsigned int _format,
				unsigned int _type,
				void * _data = NULL,
				int _param = GL_LINEAR);

	/**
	 * Create texture from a file supported by FreeImage.
	 */
	void create(const std::string& _fileName);

	void setLayer(unsigned int layer_);
	unsigned int getLayer();

	void bind() const;
	void unbind() const;
	
	//Id of this texture
	unsigned int getID() const;
private:
	void clear();
	unsigned int id_;
	unsigned int width_;
	unsigned int height_;
	unsigned int layer_;
};
#endif
