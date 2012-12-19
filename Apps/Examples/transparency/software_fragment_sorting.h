#ifndef SOFTWARE_FRAGMENT_SORTING_H
#define SOFTWARE_FRAGMENT_SORTING_H

#include "transparency_utils.h"
#include "RasterizationSpan.h"
#include "PixelFragment.h"

namespace CGoGN
{

namespace Transparency
{

/**
 * Computes the screen coordinates of v using the model-view-projection matrix mvp.
 */
static inline glm::vec4 project(glm::vec4 const & v, glm::mat4 const & mvp, glm::vec4 const & viewportCenter, glm::vec4 const & viewportScale)
{
	glm::vec4 const tmp(mvp * v);

	return viewportScale * tmp / tmp[3]  + viewportCenter;
}


typedef std::vector< RasterizationSpan > RasterizationSpans;

/**
 * Scans a side of a triangle setting firstX and lastX in spans using the Bresenham's line drawing algorithm.
 * y1 must be strictly less than y2.
 */
static void updateRasterizationSpansTopDown(RasterizationSpans & spans, int const viewportHeight,
		int x1, int y1, float const a1, float const b1, float const c1,
		int x2, int y2, float const a2, float const b2, float const c2)
{
	static bool const debug = false;

	assert(y1 < y2);

	int const sx = x2 - x1;
	int const sy = y2 - y1;
	int const dx1 = (0 < sx) - (sx < 0);
	int m = std::abs(sx);
	int n = sy;
	int dx2 = dx1;
	int dy2 = 0;

	if (m < n)
	{
		std::swap(m, n);
		dx2 = 0;
		dy2 = 1;
	}

	int x = x1;
	int y = y1;
	// previousX and previousY are part of a fix to
	// a seam problem; the goal here is to use only the leftmost
	// pixel of each segment when sx >= 0
	int previousX = x;
	int previousY = y - 1;
	int pixelCount = m + 2;
	int k = n / 2;

	while (--pixelCount)
	{
		// other variables used to fix the seam problem
		int nextX, nextY;

		k += n;

		if (k < m)
		{
			nextX = x + dx2;
			nextY = y + dy2;
		}
		else
		{
			k -= m;
			nextX = x + dx1;
			nextY = y + 1;
		}

		if (0 <= y && y < viewportHeight && ((0 <= sx && previousY < y) || (sx < 0 && (y < nextY || x == x2))))
		{
			float const s = static_cast<float>(pixelCount - 1) / std::max(m, 1);

			assert(linerp(s, a1, a2) || linerp(s, b1, b2) || linerp(s, c1, c2));

			spans[y].updateRange(x, linerp(s, a1, a2), linerp(s, b1, b2), linerp(s, c1, c2));

			if (debug)
			{
				DEBUG_OUT << x << ' ' << y << ' ' <<  spans[y].firstX() << ' ' << spans[y].lastX() << std::endl;
			}
		}

		previousX = x;
		previousY = y;
		x = nextX;
		y = nextY;
	}
}

/**
 * Calls updateRasterizationSpansTopDown with the correct orientation.
 * Handles horizontal lines.
 */
static void updateRasterizationSpans(RasterizationSpans & spans, int const viewportHeight,
		int x1, int y1, float const a1, float const b1, float const c1,
		int x2, int y2, float const a2, float const b2, float const c2)
{
	static bool const debug = false;

	if (debug)
	{
		DEBUG_OUT << x1 << ' ' << y1 << ' ' << x2 << ' ' << y2 << std::endl;
	}

	if (y1 == y2)
	{
		spans[y1].updateRange(x1, a1, b1, c1);
		spans[y2].updateRange(x2, a2, b2, c2);
	}
	else if (y1 < y2)
	{
		updateRasterizationSpansTopDown(spans, viewportHeight, x1, y1, a1, b1, c1, x2, y2, a2, b2, c2);
	}
	else
	{
		updateRasterizationSpansTopDown(spans, viewportHeight, x2, y2, a2, b2, c2, x1, y1, a1, b1, c1);
	}

	if (debug)
	{
		DEBUG_OUT << y1 << ' ' << spans[y1].firstX() << ' ' << spans[y1].lastX() << std::endl;
		DEBUG_OUT << y2 << ' ' << spans[y2].firstX() << ' ' << spans[y2].lastX() << std::endl;
	}
}

typedef std::vector< PixelFragment > PixelFragments;

typedef std::vector< PixelFragments > FragmentBuffer;

/**
 * Rasterizes triangle p0-p1-p2 by accumulating its fragments in fragmentBuffer.
 * spans and fragmentBuffer must be correctly sized.
 */
static void rasterizeTriangle(RasterizationSpans & spans, int const viewportWidth, int const viewportHeight,
		glm::vec4 const & p0, glm::vec4 const & p1, glm::vec4 const & p2,
		unsigned int rgba, FragmentBuffer & fragmentBuffer)
{
	int const x0 = roundf(p0.x);
	int const x1 = roundf(p1.x);
	int const x2 = roundf(p2.x);
	int const y0 = roundf(p0.y);
	int const y1 = roundf(p1.y);
	int const y2 = roundf(p2.y);
	int const firstY = std::max(0, std::min(std::min(y0, y1), y2));
	int const lastY = std::min(viewportHeight - 1, std::max(std::max(y0, y1), y2));

	for (int y = firstY; y <= lastY; ++y)
	{
		spans[y].reset();
	}

	updateRasterizationSpans(spans, viewportHeight,
			x1, y1, 0.0f, 1.0f, 0.0f,
			x0, y0, 1.0f, 0.0f, 0.0f);
	updateRasterizationSpans(spans, viewportHeight,
			x1, y1, 0.0f, 1.0f, 0.0f,
			x2, y2, 0.0f, 0.0f, 1.0f);
	updateRasterizationSpans(spans, viewportHeight,
			x2, y2, 0.0f, 0.0f, 1.0f,
			x0, y0, 1.0f, 0.0f, 0.0f);

	float const dz = std::abs(std::min(std::min(p0.z, p1.z), p2.z)) + 1.0f;

	for (int y = firstY + 1; y <= lastY; ++y)
	{
		RasterizationSpan const & span = spans[y];
		int const firstX = span.firstX();
		int const lastX = span.lastX();
		float const xSpan = std::max(1, lastX - firstX);

		for (int x = firstX; x < lastX; ++x)
		{
			if (0 <= x && x < viewportWidth)
			{
				float const s = (x - firstX) / xSpan;
				float const a = linerp(s, span.firstA(), span.lastA());
				float const b = linerp(s, span.firstB(), span.lastB());
				float const c = linerp(s, span.firstC(), span.lastC());
				float const z = 1.0f / (a / (p0.z + dz) + b / (p1.z + dz) + c / (p2.z + dz));

				assert(a || b || c);

#define DEBUG_HIGHLIGHT_SPAN_EXTREMITIES 0
#if DEBUG_HIGHLIGHT_SPAN_EXTREMITIES
				if (x == firstX)
				{
					fragmentBuffer[y * viewportWidth + x].push_back(PixelFragment(z, 0xFF00FF00));
				}
				else if (x >= lastX - 1)
				{
					fragmentBuffer[y * viewportWidth + x].push_back(PixelFragment(z, 0xFFFF0000));
				}
				else
#endif
#undef DEBUG_HIGHLIGHT_SPAN_EXTREMITIES
				{
					fragmentBuffer[y * viewportWidth + x].push_back(PixelFragment(z, rgba));
				}
			}
		}
	}
}

/**
 * Projects and rasterizes each of evr's triangle using VBO data.
 * fragmentBuffer must already be sized according to viewport information.
 * No shading yet (TODO).
 */
static void rasterizeTrianglesAndAccumulateFragments(Algo::Render::GL2::ExplodeVolumeAlphaRender const * const evr,
		GLint const viewport[4], glm::mat4 const & mvp, FragmentBuffer & fragmentBuffer)
{
	static bool const debugShowRasterizationProgress = false;
	GL2::VBODataPointer const colors(evr->colors());
	GL2::VBODataPointer const vertices(evr->vertices());

	if (colors && vertices)
	{
		assert(colors.elementCount() == vertices.elementCount());

		GLint const viewportX = viewport[0];
		GLint const viewportY = viewport[1];
		GLint const viewportWidth = viewport[2];
		GLint const viewportHeight = viewport[3];

		glm::vec4 const viewportCenter(viewportX + viewportWidth / 2.0, viewportY + viewportHeight / 2.0, 0.0, 0.0);
		glm::vec4 const viewportScale(viewportWidth / 2.0, viewportHeight / 2.0, 1.0, 1.0);

		RasterizationSpans spans(viewportHeight);

		fragmentBuffer.resize(viewportHeight * viewportWidth);

		for (unsigned int i = 0; i < vertices.elementCount(); i += 4)
		{
			glm::vec4 const faceCenter(colors[i * 4 + 0 + 0], colors[i * 4 + 0 + 1], colors[i * 4 + 0 + 2], 1.0);
			glm::vec4 const volumeCenter(vertices[i * 3 + 0 + 0], vertices[i * 3 + 0 + 1], vertices[i * 3 + 0 + 2], 1.0);
			glm::vec4 v1(vertices[i * 3 + 3 + 0], vertices[i * 3 + 3 + 1], vertices[i * 3 + 3 + 2], 1.0);
			glm::vec4 v2(vertices[i * 3 + 6 + 0], vertices[i * 3 + 6 + 1], vertices[i * 3 + 6 + 2], 1.0);
			glm::vec4 v3(vertices[i * 3 + 9 + 0], vertices[i * 3 + 9 + 1], vertices[i * 3 + 9 + 2], 1.0);

			v1 = project(v1, mvp, viewportCenter, viewportScale);
			v2 = project(v2, mvp, viewportCenter, viewportScale);
			v3 = project(v3, mvp, viewportCenter, viewportScale);

			v1.y = viewportHeight - 1 - v1.y;
			v2.y = viewportHeight - 1 - v2.y;
			v3.y = viewportHeight - 1 - v3.y;

			QColor const color(colors[i * 4 + 4 + 0] * 255.0, colors[i * 4 + 4 + 1] * 255.0, colors[i * 4 + 4 + 2] * 255.0, colors[i * 4 + 4 + 3] * 255.0);

			rasterizeTriangle(spans, viewportWidth, viewportHeight, v1, v2, v3, color.rgba(), fragmentBuffer);

			if (debugShowRasterizationProgress)
			{
				DEBUG_OUT << i << " / " << vertices.elementCount() << '\r' << std::flush;
			}
		}
	}
}

/**
 * Sorts each pixel's fragment and alpha-blends them; the result is stored in the appropriate pixel of image.
 * fragmentBuffer and image must have the same dimensions.
 */
static void sortAndBlend(FragmentBuffer & fragmentBuffer, QImage & image)
{
	int const viewportWidth = image.width();
	int const viewportHeight = image.height();

	for (int y = 0; y < viewportHeight; ++y)
	{
		for (int x = 0; x < viewportWidth; ++x)
		{
			PixelFragments & fragments = fragmentBuffer[y * viewportWidth + x];

			std::stable_sort(fragments.begin(), fragments.end());

			float red = 0.0f;
			float green = 0.0f;
			float blue = 0.0f;

			for (PixelFragments::const_iterator i = fragments.begin(); i != fragments.end(); ++i)
			{
				QColor const rgba(QColor::fromRgba(i->rgba()));
				float const a = rgba.alphaF();
				red = (1.0f - a) * red + a * rgba.redF();
				green = (1.0f - a) * green + a * rgba.greenF();
				blue = (1.0f - a) * blue + a * rgba.blueF();
			}

			image.setPixel(x, y, QColor(red * 255.0f, green * 255.0f, blue * 255.0f).rgba());
		}
	}
}

} /* namespace Transparency */

} /* namespace CGoGN */

#endif /* SOFTWARE_FRAGMENT_SORTING_H */
