// Copyright © 2008-2013 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef _HUDRETICLE_H
#define _HUDRETICLE_H

// HUD targeting reticle. Always sits at the center of the screen.
// - appearance is more or less a circle and matches firing arc
// - show visibly when target is inside arc
// - Draw target stats next to the crosshair: name, integrity, distance, velocity
// - Flash target name when hit
// What it's not for (use separate widget for that)
// - showing target icon/model/wireframe
// - target cargo analysis / detailed target info

#include "libs.h"
#include "text/TextureFont.h"

class Ship;

class HudReticle {
public:
	HudReticle(RefCountedPtr<Text::TextureFont>, Graphics::Renderer *);
	void BlinkTargetLabel();
	void Draw();
	void Update(const Ship* owner);

private:
	bool m_gunsLockedOn;
	Graphics::Renderer *m_renderer;
	RefCountedPtr<Text::TextureFont> m_font;
	std::string m_targetDist;
	std::string m_targetLabel;
	std::string m_targetVel;
	Uint32 m_blinkTimer;
};

#endif
