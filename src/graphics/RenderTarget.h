// Copyright © 2008-2013 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef _GRAPHICS_RENDERTARGET_H
#define _GRAPHICS_RENDERTARGET_H
/*
 * Render target. Created by filling out a description and calling
 * renderer->CreateRenderTarget.
 */
#include "libs.h"
#include "Texture.h"

namespace Graphics {

// A render target may have a color texture, depth buffer/texture or both.
// Setting the formats to NONE will skip the texture creation, and you will
// have to set the textures yourself.
// Only request allowDepthTexture if you actually need to read the depth as texture
// Specifying a depth format with no allowDepthTexture will create a depth buffer
// fixed to this rendertarget
struct RenderTargetDesc {
	RenderTargetDesc() : 
		colorFormat(TEXTURE_FORMAT_NONE), depthFormat(TEXTURE_FORMAT_NONE), allowDepthTexture(false), width(0), height(0)
	{}
	
	TextureFormat colorFormat;
	TextureFormat depthFormat;
	bool allowDepthTexture;
	Uint16 width;
	Uint16 height;
};

class RenderTarget {
public:
	virtual ~RenderTarget() { }

	virtual Texture *GetColorTexture() const = 0;
	virtual Texture *GetDepthTexture() const = 0;
	//Replace the texture attachment, or pass zero to detach
	//Increases the new texture's reference count and decreases
	//any existing texture's count
	//Setting a depth texture is not allowed if the render target is not
	//created with allowDepthTexture
	virtual void SetColorTexture(Texture*) = 0;
	virtual void SetDepthTexture(Texture*) = 0;

	const RenderTargetDesc &GetDesc() const { return m_desc; }

protected:
	RenderTarget(const RenderTargetDesc &d) : m_desc(d) { }

	RenderTargetDesc m_desc;
};

}

#endif
