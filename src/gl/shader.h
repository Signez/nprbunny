/**
 * Shader class.
 *
 * Based on EPFL "Introduction to Computer Graphics" framework, cleaned
 * and improved by authors:
 *
 * Erwan Lucas,
 * Stanislas Signoud,
 * Joachim Joyaux.
 */
#ifndef __SHADER_H__
#define __SHADER_H__
#include <string>
#include "../math/Matrix4.h"

class Shader 
{
	public:
		Shader();
		Shader(const std::string _vertexFileName, const std::string& _fragmentFileName);
		~Shader();
		void create(const std::string _vertexFileName, const std::string& _fragmentFileName);
		void bind() const;
		void unbind() const;

		void setFloatUniform(const std::string& _name, float _f) const;
		void setIntUniform(const std::string& _name, int _f) const;
		void setVector2Uniform(const std::string& _name, float _v1, float _v2) const;
		void setVector3Uniform(const std::string& _name, float _v1, float _v2, float _v3) const;
		void setVector4Uniform(const std::string& _name, float _v1, float _v2, float _v3, float _v4) const;
		void setMatrix3x3Uniform(const std::string& _name, const float *_matrix) const;
		void setMatrix3x3Uniform(const std::string& _name, const Matrix4 & _matrix) const;

		void setMatrix4x4Uniform(const std::string& _name, const float *_matrix) const;
		void setMatrix4x4Uniform(const std::string& _name, const Matrix4 & _matrix) const;

	private:
		int getUniformLocation(const std::string& _name) const;
		void clear();
		unsigned int programObj_;
};
#endif
