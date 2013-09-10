// Copyright © 2008-2013 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef OCULUSRIFT_H
#define OCULUSRIFT_H

class OculusRiftImplemetation;

class OculusRiftInterface {
public:
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
