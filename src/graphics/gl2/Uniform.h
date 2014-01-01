// Copyright © 2008-2013 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef _GL2_UNIFORM_H
#define _GL2_UNIFORM_H
/*
 * Shader uniform
 */
#include "libs.h"
namespace Graphics {

	class Texture;

	namespace GL2 {
		class Uniform {
		public:
			Uniform();
			void Init(const char *name, GLuint program);
			void Set(int);
			void Set(float);
			void Set(float, float);
			void Set(float, float, float, float);
			void Set(const vector3f&);
			void Set(const vector3d&);
			void Set(const Color&);
			void Set(const int v[3]);
			void Set(const float m[9]);
			void Set(Texture *t, unsigned int unit);
			void Set(const matrix4x4f&, const bool bTranspose = false);

		//private:
			GLint m_location;
		};
	}
}

#endif
