/**
 * Texture array definition.
 *
 * Based on EPFL "Introduction to Computer Graphics" framework, cleaned
 * and improved by authors:
 *
 * Erwan Lucas,
 * Stanislas Signoud,
 * Joachim Joyaux.
 */
#ifndef __TEXTUREARRAY_H__
#define __TEXTUREARRAY_H__
#include <string>
#include "gl.h"

class TextureArray
{
public:
	TextureArray();
	~TextureArray();

	/**
	 * Add an image to the array.
	 */
	void create(const std::string& _fileName, const std::string& _ext, int nb);

	void bind() const;
	void unbind() const;
	void setLayer(unsigned int _layer);
	unsigned int getLayer();
	
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
