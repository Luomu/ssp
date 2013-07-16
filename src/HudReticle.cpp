#include "HudReticle.h"
#include "Colors.h"
#include "graphics/Drawables.h"
#include "graphics/Graphics.h"
#include "graphics/Renderer.h"
#include "Ship.h"
#include "StringF.h"

const float RETICLE_RADIUS = 0.15f;
const Uint32 BLINK_TIME = 1500;

HudReticle::HudReticle(RefCountedPtr<Text::TextureFont> font, Graphics::Renderer *r)
: m_gunsLockedOn(false)
, m_font(font)
, m_renderer(r)
, m_blinkTimer(0)
{
}

void HudReticle::BlinkTargetLabel()
{
	m_blinkTimer = SDL_GetTicks() + BLINK_TIME;
}

void HudReticle::Update(const Ship *owner)
{
	m_targetLabel.clear();
	m_targetDist.clear();

	Body *tgt = owner->GetCombatTarget();
	if (tgt) {
		m_targetLabel = stringf("%0{u}%% %1", tgt->GetIntegrity(), tgt->GetLabel());
		m_targetDist = format_distance(tgt->GetInterpPositionRelTo(owner).Length());
		m_targetVel = stringf("%0{f.0}m/s", tgt->GetVelocityRelTo(owner).Length());
	}
	m_gunsLockedOn = owner->TargetInSight();
}

void HudReticle::Draw()
{
	const float scrAspect = Graphics::GetScreenWidth() / float(Graphics::GetScreenHeight());
	m_renderer->SetOrthographicProjection(0, Graphics::GetScreenWidth(), Graphics::GetScreenHeight(), 0, -1, 1);
	m_renderer->SetTransform(matrix4x4f::Identity());
	m_renderer->SetDepthWrite(false);

	//target info on right side of the reticle
	if (!m_targetLabel.empty()) {
		const Uint32 now = SDL_GetTicks();
		Color c = Colors::HUD_TARGET_INFO;
		if (now < m_blinkTimer && (now & 0x100) != 0) c = Color::GRAY;
		const float lineHeight = m_font->GetHeight();
		float y = Graphics::GetScreenHeight() * 0.5f - (lineHeight * 1.5f);
		const float x = Graphics::GetScreenWidth() * (0.5f + RETICLE_RADIUS/2.f);
		m_font->RenderString(m_targetLabel.c_str(), x, y, c);
		y += lineHeight;
		m_font->RenderString(m_targetDist.c_str(), x, y, c);
		y += lineHeight;
		m_font->RenderString(m_targetVel.c_str(), x, y, c); //y += lineHeight;
	}

	m_renderer->SetOrthographicProjection(-scrAspect, scrAspect, 1.f, -1.f, -1.f, 1.f);
	m_renderer->SetTransform(matrix4x4f::Identity());
	Graphics::Drawables::Circle circ(RETICLE_RADIUS, Colors::HUD_TARGET_INFO);
	circ.Draw(m_renderer);
	if (m_gunsLockedOn) {
		m_renderer->SetTransform(matrix4x4f::ScaleMatrix(0.9));
		circ.Draw(m_renderer);
	}
}
