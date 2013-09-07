// Copyright © 2008-2013 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef _WORLDVIEW_H
#define _WORLDVIEW_H

#include "libs.h"
#include "CameraController.h"
#include "EquipType.h"
#include "HudReticle.h"
#include "Serializer.h"
#include "SpeedLines.h"
#include "View.h"
#include "gui/Gui.h"
#include "gui/GuiWidget.h"

class Body;
class Frame;
class LabelSet;
class Ship;
class NavTunnelWidget;
namespace Gui { class TexturedQuad; }

class WorldView: public View {
public:
	friend class NavTunnelWidget;
	WorldView();
	WorldView(Serializer::Reader &reader);
	virtual ~WorldView();
	CameraController *GetCameraController() const { return m_activeCameraController; }
	enum CameraController::Type GetCamType() const { return m_camType; }
	int GetActiveWeapon() const;
	virtual void Draw();
	virtual void Draw3D(const ViewEye eye = ViewEye_Centre);
	virtual void Save(Serializer::Writer &wr);
	virtual void ShowAll();
	virtual void Update(const ViewEye eye = ViewEye_Centre);
	void HideTargetActions();
	void OnClickBlastoff();
	void ReportHit(const Body *b);
	void SetCamType(enum CameraController::Type);
	void ShowTargetActions();
	void ToggleTargetActions();

	sigc::signal<void> onChangeCamType;

	static const double PICK_OBJECT_RECT_SIZE;

protected:
	virtual void OnSwitchTo();
	virtual void OnSwitchFrom();

private:
	void InitObject();

	void RefreshHyperspaceButton();
	void RefreshButtonStateAndVisibility();
	void UpdateCommsOptions();

	void ChangeInternalCameraMode(InternalCameraController::Mode m);
	void UpdateCameraName();

	enum IndicatorSide {
		INDICATOR_HIDDEN,
		INDICATOR_ONSCREEN,
		INDICATOR_LEFT,
		INDICATOR_RIGHT,
		INDICATOR_TOP,
		INDICATOR_BOTTOM
	};

	struct Indicator {
		vector2f pos;
		vector2f realpos;
		IndicatorSide side;
		Gui::Label *label;
		Indicator(): pos(0.0f, 0.0f), realpos(0.0f, 0.0f), side(INDICATOR_HIDDEN), label(0) {}
	};

	const Frame* GetCurrentCamFrame(const ViewEye eye = ViewEye_Centre);
	void UpdateProjectedObjects(const ViewEye eye = ViewEye_Centre);
	void UpdateIndicator(Indicator &indicator, const vector3d &direction);
	void HideIndicator(Indicator &indicator);
	void SeparateLabels(Gui::Label *a, Gui::Label *b);

	void OnToggleLabels();
	void OnToggleCameraMagnification();

	void DrawCrosshair(float px, float py, float sz, const Color &c);
	void DrawCombatTargetIndicator(const Indicator &target, const Indicator &lead, const Color &c);
	void DrawTargetSquare(const Indicator &marker, const Color &c);
	void DrawVelocityIndicator(const Indicator &marker, const Color &c);
	void DrawImageIndicator(const Indicator &marker, Gui::TexturedQuad *quad, const Color &c);
	void DrawEdgeMarker(const Indicator &marker, const Color &c);

	Gui::Button *AddCommsOption(const std::string msg, int ypos, int optnum);
	void AddCommsNavOption(const std::string msg, Body *target);
	void OnClickCommsNavOption(Body *target);
	void BuildCommsNavOptions();

	void HideLowThrustPowerOptions();
	void ShowLowThrustPowerOptions();
	void OnClickLowThrustPower();
	void OnSelectLowThrustPower(float power);

	void OnClickHyperspace();
	void OnChangeWheelsState(Gui::MultiStateImageButton *b);
	void OnChangeFlightState(Gui::MultiStateImageButton *b);
	void OnHyperspaceTargetChanged();
	void OnPlayerDockOrUndock();
	void OnPlayerChangeTarget();
	void OnPlayerChangeFlightControlState();
	void SelectBody(Body *, bool reselectIsDeselect);
	Body* PickBody(const double screenX, const double screenY) const;
	void MouseButtonDown(int button, int x, int y);

	NavTunnelWidget *m_navTunnel;
	ScopedPtr<HudReticle> m_reticle;
	ScopedPtr<SpeedLines> m_speedLines;

	Gui::ImageButton *m_hyperspaceButton;

	Gui::Label *m_pauseText;
	Gui::Label *m_showCameraName;
	Gui::Fixed *m_commsOptions;
	Gui::VBox *m_commsNavOptions;
	Gui::HBox *m_commsNavOptionsContainer;
	Gui::Fixed *m_lowThrustPowerOptions;
	Gui::Label *m_flightStatus, *m_debugText;
	Gui::ImageButton *m_launchButton;
	Gui::MultiStateImageButton *m_wheelsButton;
	Gui::MultiStateImageButton *m_flightControlButton;
	bool m_labelsOn;
	enum CameraController::Type m_camType;
	Uint32 m_showTargetActionsTimeout;
	Uint32 m_showLowThrustPowerTimeout;
	Uint32 m_showCameraNameTimeout;

#if WITH_DEVKEYS
	Gui::Label *m_debugInfo;
#endif

	Gui::Label *m_hudVelocity, *m_hudTargetDist, *m_hudAltitude, *m_hudPressure, *m_hudHyperspaceInfo, *m_hudTargetInfo;
	Gui::MeterBar *m_hudHullTemp, *m_hudWeaponTemp, *m_hudHullIntegrity, *m_hudShieldIntegrity;
	Gui::MeterBar *m_hudTargetHullIntegrity, *m_hudTargetShieldIntegrity;
	Gui::MeterBar *m_hudFuelGauge;

	sigc::connection m_onHyperspaceTargetChangedCon;
	sigc::connection m_onPlayerChangeTargetCon;
	sigc::connection m_onChangeFlightControlStateCon;
	sigc::connection m_onMouseButtonDown;
	sigc::connection m_toggleCameraMagnificationCon;

	Gui::LabelSet *m_bodyLabels;
	std::map<Body*,vector3d> m_projectedPos;

	std::vector<Camera*> m_cameras;
	ScopedPtr<InternalCameraController> m_internalCameraController;
	ScopedPtr<ExternalCameraController> m_externalCameraController;
	ScopedPtr<SiderealCameraController> m_siderealCameraController;
	ScopedPtr<StereoCameraController>	m_stereoCameraController;
	CameraController *m_activeCameraController; //one of the above

	Indicator m_velIndicator;
	Indicator m_navVelIndicator;
	Indicator m_navTargetIndicator;
	Indicator m_combatTargetIndicator;
	Indicator m_targetLeadIndicator;
	Indicator m_mouseDirIndicator;

	ScopedPtr<Gui::TexturedQuad> m_indicatorMousedir;
	vector2f m_indicatorMousedirSize;
};

class NavTunnelWidget: public Gui::Widget {
public:
	NavTunnelWidget(WorldView *worldView);
	virtual void Draw();
	virtual void GetSizeRequested(float size[2]);
	void DrawTargetGuideSquare(const vector2f &pos, const float size, const Color &c);

private:
	WorldView *m_worldView;
};

#endif /* _WORLDVIEW_H */
