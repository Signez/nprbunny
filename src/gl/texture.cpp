/**
 * Simple texture class implementation.
 *
 * Based on EPFL "Introduction to Computer Graphics" framework, cleaned
 * and improved by authors:
 *
 * Erwan Lucas,
 * Stanislas Signoud,
 * Joachim Joyaux.
 */
#include "texture.h"
#include "FreeImage.h"
#include <fstream>
#include <cassert>
#include <iostream>

Texture::Texture() : id_(0), layer_(0), width_(0), height_(0)
{
}

Texture::Texture(unsigned int _width, 
				 unsigned int _height, 
				 unsigned int _internalFormat, 
				 unsigned int _format,
				 unsigned int _type) : id_(0), layer_(0), width_(0), height_(0)
{
	create(_width, _height, _internalFormat, _format, _type, NULL);
}

Texture::~Texture()
{
	clear();
}

void Texture::create(unsigned int _width, 
					 unsigned int _height, 
					 unsigned int _internalFormat, 
					 unsigned int _format,
					 unsigned int _type,
					 void * _data,
					 int _param)
{
	assert(_width <= 4096 && _height <= 4096);
	clear();
	width_ = _width;
	height_ = _height;
	glGenTextures(1, &id_);
	assert(id_ != 0);
	glBindTexture(GL_TEXTURE_2D, id_);
	glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, width_, height_, 0, _format, _type, _data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _param);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _param);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0); 
}

void Texture::create(const std::string& _filePath)
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib(0);

	fif = FreeImage_GetFileType(_filePath.c_str(), 0); // Check the file signature and deduce its format

	if (fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(_filePath.c_str());

	if (fif == FIF_UNKNOWN) { // If still unkown, return failure
		std::cerr << "E: Couldn't load file '" << _filePath << "' as texture." << std::endl;
		assert(fif != FIF_UNKNOWN);
	}

	if (FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
		dib = FreeImage_Load(fif, _filePath.c_str());

	if (!dib) {
		std::cerr << "E: Couldn't load file '" << _filePath << "' as texture." << std::endl;
		assert(!dib);
	}

	BYTE* data = FreeImage_GetBits(dib); // Retrieve the image data

	width_ = FreeImage_GetWidth(dib); // Get the image width and height
	height_ = FreeImage_GetHeight(dib);
	int iBPP = FreeImage_GetBPP(dib);
	glGenTextures(1, &id_);
	assert(id_ != 0);
	glBindTexture(GL_TEXTURE_2D, id_);

	int iFormat = iBPP == 24 ? GL_BGR : iBPP == 8 ? GL_LUMINANCE : 0;

	assert(iFormat != 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, iFormat, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	FreeImage_Unload(dib);

	std::cerr << "I: Texture '" << _filePath << "' loaded." << std::endl;
}
void Texture::bind() const
{
	assert(id_ != 0);
	glEnable(GL_TEXTURE_2D); 
	glActiveTextureARB(GL_TEXTURE0_ARB+layer_);
	glBindTexture(GL_TEXTURE_2D, id_);
}

void Texture::unbind() const
{
	assert(id_ != 0);
	glActiveTextureARB(GL_TEXTURE0_ARB+layer_);
	glBindTexture(GL_TEXTURE_2D, 0); 
	glDisable(GL_TEXTURE_2D);
}

void Texture::setLayer(unsigned int _layer)
{
	layer_ = _layer;
}

unsigned int Texture::getLayer()
{
	return layer_;
}

unsigned int Texture::getID() const
{
	return id_;
}

void Texture::clear()
{
	if(id_ != 0)
	{
		glDeleteTextures(1, &id_);
		id_ = 0;
		layer_ = 0;
		width_ = 0;
		height_ = 0;
	}
}
