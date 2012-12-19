#ifndef DEPTH_PEELING_H
#define DEPTH_PEELING_H

#include "Utils/textureSticker.h"

namespace CGoGN
{

namespace VolumeExplorerTools
{

static void peelDepthLayerAndBlendToDefaultBuffer(CGoGN::Utils::FBO * const previousFBO, CGoGN::Utils::FBO * const currentFBO,
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
	glFlush(); DEBUG_GL;
	currentFBO->Unbind(); DEBUG_GL;
	glDrawBuffer(GL_BACK);

	glDisable(GL_DEPTH_TEST); DEBUG_GL;
	glEnable(GL_BLEND); DEBUG_GL;
	glBlendEquation(GL_FUNC_ADD); DEBUG_GL;
	glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA); DEBUG_GL;
	Utils::TextureSticker::StickTextureOnWholeScreen(currentFBO->GetColorTexId(0));
}

} /* namespace VolumeExplorerTools */

} /* namespace CGoGN */

#endif /* DEPTH_PEELING_H */
