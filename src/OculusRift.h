// Copyright © 2008-2013 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef OCULUSRIFT_H
#define OCULUSRIFT_H

class OculusRiftImplemetation;

class OculusRiftInterface {
public:
	//-----------------------------------------------------------------------------------
	// ***** Viewport

	// Viewport describes a rectangular area used for rendering, in pixels.
	struct Viewport
	{
		int x, y;
		int w, h;

		Viewport() {}
		Viewport(int x1, int y1, int w1, int h1) : x(x1), y(y1), w(w1), h(h1) { }

		bool operator == (const Viewport& vp) const
		{ return (x == vp.x) && (y == vp.y) && (w == vp.w) && (h == vp.h); }
		bool operator != (const Viewport& vp) const
		{ return !operator == (vp); }
	};

	static void Init();
	static void Uninit();
	static void Update();

	static bool HasHMD();

	static void GetYawPitchRoll(float &yaw, float &pitch, float &roll);

	static void GetDistortionValues(float &XCenterOffset, float &Scale, float &K0, float &K1, float &K2, float &K3);
private:
	static ScopedPtr<OculusRiftImplemetation> mPimpl;
};

#endif // OCULUSRIFT_H
