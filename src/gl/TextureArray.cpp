/**
 * Texture Array implementation.
 *
 * Based on EPFL "Introduction to Computer Graphics" framework, cleaned
 * and improved by authors:
 *
 * Erwan Lucas,
 * Stanislas Signoud,
 * Joachim Joyaux.
 */
#include "TextureArray.h"
#include "FreeImage.h"
#include <fstream>
#include <cassert>
#include <iostream>
#include <sstream>

TextureArray::TextureArray() : id_(0), width_(0), height_(0), layer_(0)
{
}

TextureArray::~TextureArray()
{
	clear();
}

void TextureArray::create(const std::string& _filePath, const std::string& _ext, int nb)
{
	for (int i = 0; i < nb; i++) {
		std::ostringstream builder;
		builder << _filePath << i << _ext;
		std::string currentFilePath = builder.str();

		FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
		FIBITMAP* dib(0);

		fif = FreeImage_GetFileType(currentFilePath.c_str(), 0); // Check the file signature and deduce its format

		if (fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
			fif = FreeImage_GetFIFFromFilename(currentFilePath.c_str());

		if (fif == FIF_UNKNOWN) { // If still unknown, return failure
			std::cerr << "E: Couldn't load file '" << currentFilePath << "' as texture." << std::endl;
			assert(fif != FIF_UNKNOWN);
		}

		if (FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
			dib = FreeImage_Load(fif, currentFilePath.c_str());

		if (!dib) {
			std::cerr << "E: Couldn't load file '" << currentFilePath << "' as texture." << std::endl;
			assert(!dib);
		}

		BYTE* data = FreeImage_GetBits(dib); // Retrieve the image data

		width_ = FreeImage_GetWidth(dib); // Get the image width and height
		height_ = FreeImage_GetHeight(dib);
		int iBPP = FreeImage_GetBPP(dib);

		int iFormat = iBPP == 24 ? GL_BGR : iBPP == 32 ? GL_RGBA : iBPP == 8 ? GL_LUMINANCE : 0;
		assert(iFormat != 0);

		if(i == 0) {
			glGenTextures(1, &id_);
			glBindTexture(GL_TEXTURE_2D_ARRAY, id_);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT );

			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB, width_, height_, nb, 0, iFormat, GL_UNSIGNED_BYTE, 0);
		}

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width_, height_, 1,
				iFormat, GL_UNSIGNED_BYTE, data);

		FreeImage_Unload(dib);

		std::cerr << "I: Texture '" << currentFilePath << "' loaded (multi)." << std::endl;
	}
}

void TextureArray::bind() const
{
	assert(id_ != 0);
	//glEnable(GL_TEXTURE_2D_ARRAY);
	glActiveTextureARB(GL_TEXTURE0_ARB  + layer_);
	glBindTexture(GL_TEXTURE_2D_ARRAY, id_);
	//std::cerr << "I: Bound." << std::endl;
}

void TextureArray::unbind() const
{
	assert(id_ != 0);
	glActiveTextureARB(GL_TEXTURE0_ARB + layer_);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	//glDisable(GL_TEXTURE_2D_ARRAY);
}

unsigned int TextureArray::getID() const
{
	return id_;
}

void TextureArray::setLayer(unsigned int _layer)
{
	layer_ = _layer;
}

unsigned int TextureArray::getLayer()
{
	return layer_;
}

void TextureArray::clear()
{
	if(id_ != 0)
	{
		glDeleteTextures(1, &id_);
		id_ = 0;
		width_ = 0;
		height_ = 0;
	}
}
