// Copyright © 2008-2013 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "CameraController.h"
#include "Ship.h"
#include "AnimationCurves.h"
#include "Pi.h"
#include "Game.h"

CameraController::CameraController(Camera *camera, const Ship *ship) :
	m_camera(camera),
	m_ship(ship),
	m_pos(0.0),
	m_orient(matrix3x3d::Identity())
{
}
#pragma optimize("",off)
void CameraController::Update()
{
	m_camera->SetFrame(m_ship->GetFrame());

	// interpolate between last physics tick position and current one,
	// to remove temporal aliasing
	const matrix3x3d &m = m_ship->GetInterpOrient();
	m_camera->SetOrient(m * m_orient);
	m_camera->SetPosition(m * m_pos + m_ship->GetInterpPosition());
}

InternalCameraController::InternalCameraController(Camera *camera, const Ship *ship) :
	CameraController(camera, ship)
{
	SetMode(MODE_FRONT);
	m_magnify = false;
	m_fov = camera->GetDefaultFov();
	m_fovTo = camera->GetDefaultFov();
}

void InternalCameraController::SetMode(Mode m)
{
	m_mode = m;
	switch (m_mode) {
		case MODE_FRONT:
			m_name = Lang::CAMERA_FRONT_VIEW;
			SetOrient(matrix3x3d::RotateY(M_PI*2));
			break;
		case MODE_REAR:
			m_name = Lang::CAMERA_REAR_VIEW;
			SetOrient(matrix3x3d::RotateY(M_PI));
			break;
		case MODE_LEFT:
			m_name = Lang::CAMERA_LEFT_VIEW;
			SetOrient(matrix3x3d::RotateY((M_PI/2)*3));
			break;
		case MODE_RIGHT:
			m_name = Lang::CAMERA_RIGHT_VIEW;
			SetOrient(matrix3x3d::RotateY(M_PI/2));
			break;
		case MODE_TOP:
			m_name = Lang::CAMERA_TOP_VIEW;
			SetOrient(matrix3x3d::RotateX((M_PI/2)*3));
			break;
		case MODE_BOTTOM:
			m_name = Lang::CAMERA_BOTTOM_VIEW;
			SetOrient(matrix3x3d::RotateX(M_PI/2));
			break;
	}
}

void InternalCameraController::Save(Serializer::Writer &wr)
{
	wr.Int32(m_mode);
}

void InternalCameraController::Load(Serializer::Reader &rd)
{
	SetMode(static_cast<Mode>(rd.Int32()));
}

void InternalCameraController::ToggleMagnification()
{
	m_magnify = !m_magnify;
	m_fovTo = m_magnify ? m_camera->GetZoomedInFov() : m_camera->GetDefaultFov();
}

void InternalCameraController::ZoomEventUpdate(float frameTime)
{
	AnimationCurves::Approach(m_fov, m_fovTo, frameTime);
	m_camera->SetFov(m_fov);
}
#pragma optimize("",off)
void InternalCameraController::Update()
{
	SetPosition(GetShip()->GetShipType()->cameraOffset);

	CameraController::Update();
}

ExternalCameraController::ExternalCameraController(Camera *camera, const Ship *ship) :
	MoveableCameraController(camera, ship),
	m_dist(200), m_distTo(m_dist),
	m_rotX(0),
	m_rotY(0),
	m_extOrient(matrix3x3d::Identity())
{
}

void ExternalCameraController::RotateUp(float frameTime)
{
	m_rotX -= 45*frameTime;
}

void ExternalCameraController::RotateDown(float frameTime)
{
	m_rotX += 45*frameTime;
}

void ExternalCameraController::RotateLeft(float frameTime)
{
	m_rotY -= 45*frameTime;
}

void ExternalCameraController::RotateRight(float frameTime)
{
	m_rotY += 45*frameTime;
}

void ExternalCameraController::ZoomIn(float frameTime)
{
	ZoomOut(-frameTime);
}

void ExternalCameraController::ZoomOut(float frameTime)
{
	m_dist += 400*frameTime;
	m_dist = std::max(GetShip()->GetClipRadius(), m_dist);
	m_distTo = m_dist;
}

void ExternalCameraController::ZoomEvent(float amount)
{
	m_distTo += 400*amount;
	m_distTo = std::max(GetShip()->GetClipRadius(), m_distTo);
}

void ExternalCameraController::ZoomEventUpdate(float frameTime)
{
	AnimationCurves::Approach(m_dist, m_distTo, frameTime);
	m_dist = std::max(GetShip()->GetClipRadius(), m_dist);
}

void ExternalCameraController::Reset()
{
	m_dist = 200;
	m_distTo = m_dist;
}

void ExternalCameraController::Update()
{
	const Ship *ship = GetShip();
	const Body *tgt = ship->GetCombatTarget();

	//always pad lock target
	if (tgt) {
		// player is between camera and target
		const matrix3x3d &m = ship->GetInterpOrient();
		const double crad = ship->GetClipRadius();
		vector3d offset = tgt->GetInterpPositionRelTo(ship);
		offset = offset.Normalized() * -4.0 * crad;
		offset += (m * vector3d(0,crad,0));

		const vector3d eye = ship->GetInterpPosition() + offset;
		const vector3d target = tgt->GetInterpPosition();
		const vector3d up = m.VectorY();//.Normalized();

		m_extOrient = matrix3x3d::LookAt(eye, target, up);

		m_camera->SetFrame(ship->GetFrame());
		m_camera->SetOrient(m_extOrient);
		m_camera->SetPosition(eye);
	} else {
		// when landed don't let external view look from below
		if (ship->GetFlightState() == Ship::LANDED ||
			ship->GetFlightState() == Ship::DOCKED) {
			m_rotX = Clamp(m_rotX, -170.0, -10.0);
		}

		vector3d p = vector3d(0, 0, m_dist);
		p = matrix3x3d::RotateX(-DEG2RAD(m_rotX)) * p;
		p = matrix3x3d::RotateY(-DEG2RAD(m_rotY)) * p;
		m_extOrient = matrix3x3d::RotateY(-DEG2RAD(m_rotY)) *
			matrix3x3d::RotateX(-DEG2RAD(m_rotX));

		SetPosition(p);
		SetOrient(m_extOrient);

		CameraController::Update();
	}
}

void ExternalCameraController::Save(Serializer::Writer &wr)
{
	wr.Double(m_rotX);
	wr.Double(m_rotY);
	wr.Double(m_dist);
}

void ExternalCameraController::Load(Serializer::Reader &rd)
{
	m_rotX = rd.Double();
	m_rotY = rd.Double();
	m_dist = rd.Double();
	m_distTo = m_dist;
}

SiderealCameraController::SiderealCameraController(Camera *camera, const Ship *ship) :
	MoveableCameraController(camera, ship),
	m_dist(200), m_distTo(m_dist),
	m_sidOrient(matrix3x3d::Identity())
{
}

void SiderealCameraController::RotateUp(float frameTime)
{
	const vector3d rotAxis = m_sidOrient.VectorX();
	m_sidOrient = matrix3x3d::Rotate(-M_PI/4 * frameTime, rotAxis) * m_sidOrient;
}

void SiderealCameraController::RotateDown(float frameTime)
{
	const vector3d rotAxis = m_sidOrient.VectorX();
	m_sidOrient = matrix3x3d::Rotate(M_PI/4 * frameTime, rotAxis) * m_sidOrient;
}

void SiderealCameraController::RotateLeft(float frameTime)
{
	const vector3d rotAxis = m_sidOrient.VectorY();
	m_sidOrient = matrix3x3d::Rotate(-M_PI/4 * frameTime, rotAxis) * m_sidOrient;
}

void SiderealCameraController::RotateRight(float frameTime)
{
	const vector3d rotAxis = m_sidOrient.VectorY();
	m_sidOrient = matrix3x3d::Rotate(M_PI/4 * frameTime, rotAxis) * m_sidOrient;
}

void SiderealCameraController::ZoomIn(float frameTime)
{
	ZoomOut(-frameTime);
}

void SiderealCameraController::ZoomOut(float frameTime)
{
	m_dist += 400*frameTime;
	m_dist = std::max(GetShip()->GetClipRadius(), m_dist);
	m_distTo = m_dist;
}

void SiderealCameraController::ZoomEvent(float amount)
{
	m_distTo += 400*amount;
	m_distTo = std::max(GetShip()->GetClipRadius(), m_distTo);
}

void SiderealCameraController::ZoomEventUpdate(float frameTime)
{
	AnimationCurves::Approach(m_dist, m_distTo, frameTime, 4.0, 50./std::max(m_distTo, 1e-7));		// std::max() here just avoid dividing by 0.
	m_dist = std::max(GetShip()->GetClipRadius(), m_dist);
}

void SiderealCameraController::RollLeft(float frameTime)
{
	const vector3d rotAxis = m_sidOrient.VectorZ();
	m_sidOrient = matrix3x3d::Rotate(M_PI/4 * frameTime, rotAxis) * m_sidOrient;
}

void SiderealCameraController::RollRight(float frameTime)
{
	const vector3d rotAxis = m_sidOrient.VectorZ();
	m_sidOrient = matrix3x3d::Rotate(-M_PI/4 * frameTime, rotAxis) * m_sidOrient;
}

void SiderealCameraController::Reset()
{
	m_dist = 200;
	m_distTo = m_dist;
}

void SiderealCameraController::Update()
{
	const Ship *ship = GetShip();

	m_sidOrient.Renormalize();			// lots of small rotations
	matrix3x3d shipOrient = ship->GetInterpOrientRelTo(Pi::game->GetSpace()->GetRootFrame());

	SetPosition(shipOrient.Transpose() * m_sidOrient.VectorZ() * m_dist);
	SetOrient(shipOrient.Transpose() * m_sidOrient);

	CameraController::Update();
}

void SiderealCameraController::Save(Serializer::Writer &wr)
{
	for (int i = 0; i < 9; i++) wr.Double(m_sidOrient[i]);
	wr.Double(m_dist);
}

void SiderealCameraController::Load(Serializer::Reader &rd)
{
	for (int i = 0; i < 9; i++) m_sidOrient[i] = rd.Double();
	m_dist = rd.Double();
	m_distTo = m_dist;
}

StereoCameraController::StereoCameraController(std::vector<Camera*> &cameras, const Ship *ship) :
	CameraController(cameras[0], ship), m_cameras(cameras)
{
	m_magnify = false;
	m_fov = cameras[0]->GetDefaultFov();
	m_fovTo = cameras[0]->GetDefaultFov();
}

void StereoCameraController::Save(Serializer::Writer &wr)
{
}

void StereoCameraController::Load(Serializer::Reader &rd)
{
}

void StereoCameraController::ToggleMagnification()
{
	m_magnify = !m_magnify;
	m_fovTo = m_magnify ? m_camera->GetZoomedInFov() : m_camera->GetDefaultFov();
}

void StereoCameraController::ZoomEventUpdate(float frameTime)
{
	AnimationCurves::Approach(m_fov, m_fovTo, frameTime);
	for(std::vector<Camera*>::iterator it = m_cameras.begin(), itEnd = m_cameras.end(); it!=itEnd; ++it) {
		(*it)->SetFov(m_fov);
	}
	
}
#pragma optimize("",off)
static double eye_offset_scale = 0.0;
void StereoCameraController::Update()
{
	SetPosition(GetShip()->GetShipType()->cameraOffset);

	const Ship* ship = GetShip();
	const matrix3x3d &orient = GetOrient();
	const vector3d pos = GetPosition();
	const vector3d offsetAxis = eye_offset_scale * orient.VectorX().Normalized();
	const vector3d shipInterpPosition = ship->GetInterpPosition();

	// interpolate between last physics tick position and current one,
	// to remove temporal aliasing
	const matrix3x3d &m = ship->GetInterpOrient();
	const matrix3x3d finalorient = (m * orient);
	
	
	assert(m_cameras.size()==2);
	{
		m_cameras[0]->SetFrame(ship->GetFrame());
		m_cameras[0]->SetOrient(finalorient);
		const vector3d finalpos = (m * (pos - offsetAxis) + shipInterpPosition);
		m_cameras[0]->SetPosition(finalpos);
	}

	{
		m_cameras[1]->SetFrame(ship->GetFrame());
		m_cameras[1]->SetOrient(finalorient);
		const vector3d finalpos = (m * (pos + offsetAxis) + shipInterpPosition);
		m_cameras[1]->SetPosition(finalpos);
	}

	//CameraController::Update();
}
