// Copyright © 2008-2013 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "Uniform.h"
#include "graphics/TextureGL.h"

namespace Graphics {
namespace GL2 {

Uniform::Uniform()
: m_location(-1)
{
}

void Uniform::Init(const char *name, GLuint program)
{
	m_location = glGetUniformLocation(program, name);
}

void Uniform::Set(int i)
{
	if (m_location != -1)
		glUniform1i(m_location, i);
}

void Uniform::Set(float f)
{
	if (m_location != -1)
		glUniform1f(m_location, f);
}

void Uniform::Set(float f1, float f2)
{
	if (m_location != -1)
		glUniform2f(m_location, f1, f2);
}

void Uniform::Set(float f1, float f2, float f3, float f4)
{
	if (m_location != -1)
		glUniform4f(m_location, f1, f2, f3, f4);
}

void Uniform::Set(const vector3f &v)
{
	if (m_location != -1)
		glUniform3f(m_location, v.x, v.y, v.z);
}

void Uniform::Set(const vector3d &v)
{
	if (m_location != -1)
		glUniform3f(m_location, v.x, v.y, v.z); //yes, 3f
}

void Uniform::Set(const Color &c)
{
	Color4f c4f = c.ToColor4f();
	if (m_location != -1)
		glUniform4f(m_location, c4f.r, c4f.g, c4f.b, c4f.a);
}

void Uniform::Set(const int v[3])
{
	if (m_location != -1)
		glUniform3i(m_location, v[0],v[1],v[2]);
}

void Uniform::Set(const float m[9])
{
	if (m_location != -1)
		glUniformMatrix3fv(m_location, 1, false, m);
}

void Uniform::Set(Texture *tex, unsigned int unit)
{
	if (m_location != -1 && tex) {
		glActiveTexture(GL_TEXTURE0 + unit);
		static_cast<TextureGL*>(tex)->Bind();
		glUniform1i(m_location, unit);
	}
}

void Uniform::Set(const matrix4x4f &mat, const bool bTranspose /*= false*/)
{
	if (m_location != -1)
		glUniformMatrix4fv(m_location, 1, bTranspose, mat.Data());
}

}
}
