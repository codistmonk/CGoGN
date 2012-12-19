#ifndef DEPTH_PEELING_H
#define DEPTH_PEELING_H

#include "Utils/textureSticker.h"

namespace CGoGN
{

namespace Transparency
{

static void peelOneDepthLayerAndBlendToDefaultBuffer(CGoGN::Utils::FBO * const previousFBO, CGoGN::Utils::FBO * const currentFBO,
		Algo::Render::GL2::ExplodeVolumeAlphaRender * const evr)
{
	currentFBO->Bind(); DEBUG_GL;
	currentFBO->EnableColorAttachments(); DEBUG_GL;
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); DEBUG_GL;
	glDisable(GL_BLEND); DEBUG_GL;
	glEnable(GL_DEPTH_TEST); DEBUG_GL;
	glActiveTexture(GL_TEXTURE0 + 0); DEBUG_GL;
	glBindTexture(GL_TEXTURE_2D, *(previousFBO->GetDepthTexId())); DEBUG_GL;
	evr->drawFaces(); DEBUG_GL;
	glBindTexture(GL_TEXTURE_2D, 0); DEBUG_GL;
	currentFBO->SafeUnbind(); DEBUG_GL;

	glDisable(GL_DEPTH_TEST); DEBUG_GL;
	glEnable(GL_BLEND); DEBUG_GL;
	glBlendEquation(GL_FUNC_ADD); DEBUG_GL;
	glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA); DEBUG_GL;
	Utils::TextureSticker::StickTextureOnWholeScreen(currentFBO->GetColorTexId(0));
}

/**
 * Performs layerCount iterations of depth peeling to render evr.
 * fbo2's depth buffer is used as initial depth buffer.
 * Upon successfull completion, all fbos are unbound.
 */
static void peelDepthLayersAndBlendToDefaultBuffer(CGoGN::Utils::FBO * const fbo1, CGoGN::Utils::FBO * const fbo2,
		Algo::Render::GL2::ExplodeVolumeAlphaRender * const evr, int const layerCount)
{
	evr->shaderFaces()->setDepthPeeling(1);
	glDisable(GL_BLEND); DEBUG_GL;
	glEnable(GL_DEPTH_TEST); DEBUG_GL;
	glActiveTexture(GL_TEXTURE0 + 0); DEBUG_GL;
	glBindTexture(GL_TEXTURE_2D, *(fbo2->GetDepthTexId())); DEBUG_GL;
	evr->drawFaces(); DEBUG_GL;
	glBindTexture(GL_TEXTURE_2D, 0); DEBUG_GL;
	fbo1->SafeUnbind(); DEBUG_GL;

	Utils::TextureSticker::StickTextureOnWholeScreen(fbo1->GetColorTexId(0));

	CGoGN::Utils::FBO * previousFBO = fbo1;
	CGoGN::Utils::FBO * currentFBO = fbo2;

	evr->shaderFaces()->setDepthPeeling(2);

	for (int i = 0; i < layerCount; ++i)
	{
		peelOneDepthLayerAndBlendToDefaultBuffer(previousFBO, currentFBO, evr);

		std::swap(previousFBO, currentFBO);
	}

	evr->shaderFaces()->setDepthPeeling(0);
}

} /* namespace Transparency */

} /* namespace CGoGN */

#endif /* DEPTH_PEELING_H */
