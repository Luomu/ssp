// Copyright © 2008-2013 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "HMDWarpMaterial.h"
#include "GeoSphere.h"
#include "Camera.h"
#include "StringF.h"
#include "graphics/Graphics.h"
#include "graphics/RendererGL2.h"
#include "OculusRift.h"
#include <sstream>

namespace Graphics {
namespace GL2 {

HMDWarpProgram::HMDWarpProgram(const std::string &filename, const std::string &defines)
{
	m_name = filename;
	m_defines = defines;
	LoadShaders(filename, defines);
	InitUniforms();
}

void HMDWarpProgram::InitUniforms()
{
	Program::InitUniforms();
	LensCenter.Init("LensCenter", m_program);
	ScreenCenter.Init("ScreenCenter", m_program);
	Scale.Init("Scale", m_program);
	ScaleIn.Init("ScaleIn", m_program);
	HmdWarpParam.Init("HmdWarpParam", m_program);
	ChromAbParam.Init("ChromAbParam", m_program);
	Texm.Init("Texm", m_program);
}

Program *HMDWarpMaterial::CreateProgram(const MaterialDescriptor &desc)
{
	assert((desc.effect == EFFECT_HMDWARP));
	std::stringstream ss;
	return new Graphics::GL2::HMDWarpProgram("HMDWarp", ss.str());
}

void HMDWarpMaterial::Apply()
{
	SetGSUniforms();
}

void HMDWarpMaterial::SetGSUniforms()
{
	HMDWarpProgram *p = static_cast<HMDWarpProgram*>(m_program);

	p->Use();
	p->emission.Set(this->emissive);
	p->sceneAmbient.Set(m_renderer->GetAmbientColor());
	//p->atmosColor.Set(ap.atmosCol);
	const float WindowWidth = Graphics::GetScreenWidth();
	const float WindowHeight = Graphics::GetScreenHeight();
	const float w = 1.0f,
          h = 1.0f,
          x = 0.0f,
          y = 0.0f;

	/*float w = float(VP.w) / float(WindowWidth),
          h = float(VP.h) / float(WindowHeight),
          x = float(VP.x) / float(WindowWidth),
          y = float(VP.y) / float(WindowHeight);*/

    const float as = float(WindowWidth) / float(WindowHeight);

	float XCenterOffset;
	float Scale;
	float K0,K1,K2,K3;
	OculusRiftInterface::GetDistortionValues(XCenterOffset, Scale, K0, K1, K2, K3);

    // We are using 1/4 of DistortionCenter offset value here, since it is
    // relative to [-1,1] range that gets mapped to [0, 0.5].
	
	p->LensCenter.Set(x + (w + XCenterOffset * 0.5f)*0.5f, y + h*0.5f);
	p->ScreenCenter.Set(x + w*0.5f, y + h*0.5f);

    // MA: This is more correct but we would need higher-res texture vertically; we should adopt this
    // once we have asymmetric input texture scale.
    const float scaleFactor = 1.0f / Scale;

    p->Scale.Set(  (w/2) * scaleFactor, (h/2) * scaleFactor * as);
    p->ScaleIn.Set((2/w),               (2/h) / as);

    p->HmdWarpParam.Set(K0, K1, K2, K3);

    /*if (PostProcessShaderRequested == PostProcessShader_DistortionAndChromAb)
    {
        pPostProcessShader->SetUniform4f("ChromAbParam",
                                        Distortion.ChromaticAberration[0], 
                                        Distortion.ChromaticAberration[1],
                                        Distortion.ChromaticAberration[2],
                                        Distortion.ChromaticAberration[3]);
    }*/

	float texf[16]={w  , 0.f, 0.f, x,
                    0.f, h  , 0.f, y,
                    0.f, 0.f, 0.f, 0.f,
					0.f, 0.f, 0.f, 1.f};
    matrix4x4f texm(texf);
    p->Texm.Set(texm);
}

/*void RenderDevice::FinishScene1()
{
    float r, g, b, a;
    DistortionClearColor.GetRGBA(&r, &g, &b, &a);
    Clear(r, g, b, a);

    float w = float(VP.w) / float(WindowWidth),
          h = float(VP.h) / float(WindowHeight),
          x = float(VP.x) / float(WindowWidth),
          y = float(VP.y) / float(WindowHeight);

    float as = float(VP.w) / float(VP.h);

    // We are using 1/4 of DistortionCenter offset value here, since it is
    // relative to [-1,1] range that gets mapped to [0, 0.5].
    pPostProcessShader->SetUniform2f("LensCenter",
                                     x + (w + Distortion.XCenterOffset * 0.5f)*0.5f, y + h*0.5f);
    pPostProcessShader->SetUniform2f("ScreenCenter", x + w*0.5f, y + h*0.5f);

    // MA: This is more correct but we would need higher-res texture vertically; we should adopt this
    // once we have asymmetric input texture scale.
    float scaleFactor = 1.0f / Distortion.Scale;

    pPostProcessShader->SetUniform2f("Scale",   (w/2) * scaleFactor, (h/2) * scaleFactor * as);
    pPostProcessShader->SetUniform2f("ScaleIn", (2/w),               (2/h) / as);

    pPostProcessShader->SetUniform4f("HmdWarpParam",
                                     Distortion.K[0], Distortion.K[1], Distortion.K[2], Distortion.K[3]);

    if (PostProcessShaderRequested == PostProcessShader_DistortionAndChromAb)
    {
        pPostProcessShader->SetUniform4f("ChromAbParam",
                                        Distortion.ChromaticAberration[0], 
                                        Distortion.ChromaticAberration[1],
                                        Distortion.ChromaticAberration[2],
                                        Distortion.ChromaticAberration[3]);
    }

    Matrix4f texm(w, 0, 0, x,
                  0, h, 0, y,
                  0, 0, 0, 0,
                  0, 0, 0, 1);
    pPostProcessShader->SetUniform4x4f("Texm", texm);

    Matrix4f view(2, 0, 0, -1,
                  0, 2, 0, -1,
                   0, 0, 0, 0,
                   0, 0, 0, 1);

    ShaderFill fill(pPostProcessShader);
    fill.SetTexture(0, pSceneColorTex);
    RenderWithAlpha(&fill, pFullScreenVertexBuffer, NULL, view, 0, 4, Prim_TriangleStrip);
}*/

}	// namespace GL2
}	// namespace Graphics
