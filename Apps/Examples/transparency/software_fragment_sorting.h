#ifndef SOFTWARE_FRAGMENT_SORTING_H
#define SOFTWARE_FRAGMENT_SORTING_H

#include "transparency_utils.h"

namespace CGoGN
{

namespace VolumeExplorerTools
{

static inline glm::vec4 project(glm::vec4 const & v, glm::mat4 const & mvp, glm::vec4 const & viewportCenter, glm::vec4 const & viewportScale)
{
	glm::vec4 const tmp(mvp * v);

	return viewportScale * tmp / tmp[3]  + viewportCenter;
}

class RasterizationSpan
{

	int m_firstX, m_lastX;

	// Barycentric coordinates in triangle
	float m_firstA, m_firstB, m_firstC;
	float m_lastA, m_lastB, m_lastC;

public:

	void updateRange(int const x, float const a, float const b, float const c)
	{
		if (x < m_firstX)
		{
			m_firstX = x;
			m_firstA = a;
			m_firstB = b;
			m_firstC = c;
		}

		if (m_lastX < x)
		{
			m_lastX = x;
			m_lastA = a;
			m_lastB = b;
			m_lastC = c;
		}
	}

	int firstX() const
	{
		return m_firstX;
	}

	float firstA() const
	{
		return m_firstA;
	}

	float firstB() const
	{
		return m_firstB;
	}

	float firstC() const
	{
		return m_firstC;
	}

	int lastX() const
	{
		return m_lastX;
	}

	float lastA() const
	{
		return m_lastA;
	}

	float lastB() const
	{
		return m_lastB;
	}

	float lastC() const
	{
		return m_lastC;
	}

	void reset()
	{
		m_firstX = INT_MAX;
		m_lastX = INT_MIN;
	}

};

typedef std::vector< RasterizationSpan > RasterizationSpans;

static inline float linerp(float const s, float const a, float const b)
{
	return s * a + (1.0f - s) * b;
}

/**
 * Scans a side of a triangle setting firstX and lastX in ContourX
 * using the Bresenham's line drawing algorithm.
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
	// XXX previousX and previousY are part of a 'dirty fix' to
	//     a seam problem; the goal here is to use only the leftmost
	//     pixel of each segment when sx >= 0
	int previousX = x;
	int previousY = y - 1;
	int pixelCount = m + 2;
	int k = n / 2;

	while (--pixelCount)
	{
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

class PixelFragment
{

	float m_z;

	int m_rgba;

public:

	PixelFragment(): m_z(-INFINITY), m_rgba(0)
	{
		// NOP
	}

	PixelFragment(float const z, int const rgba): m_z(z), m_rgba(rgba)
	{
		// NOP
	}

	float z() const
	{
		return m_z;
	}

	int rgba() const
	{
		return m_rgba;
	}

	bool operator<(PixelFragment const & that) const
	{
		return this->z() > that.z();
	}

};

typedef std::vector< PixelFragment > FragmentStack;
typedef std::vector< FragmentStack > FragmentBuffer;

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

static void sortAndBlend(FragmentBuffer & fragmentBuffer, QImage & image)
{
	int const viewportWidth = image.width();
	int const viewportHeight = image.height();

	for (int y = 0; y < viewportHeight; ++y)
	{
		for (int x = 0; x < viewportWidth; ++x)
		{
			FragmentStack & fragments = fragmentBuffer[y * viewportWidth + x];

			std::stable_sort(fragments.begin(), fragments.end());

			float red = 0.0f;
			float green = 0.0f;
			float blue = 0.0f;

			for (FragmentStack::const_iterator i = fragments.begin(); i != fragments.end(); ++i)
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

static void rasterizeTrianglesAndAccumulateFragments(Algo::Render::GL2::ExplodeVolumeAlphaRender const * const evr,
		GLint const viewport[4], glm::mat4 const & mvp, FragmentBuffer & fragmentBuffer)
{
	static bool const debugShowRasterizationProgress = false;
	VBODataPointer const colors(evr->colors());
	VBODataPointer const vertices(evr->vertices());

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

static inline void clearStacks(FragmentBuffer & fragmentBuffer)
{
	for (FragmentBuffer::iterator i = fragmentBuffer.begin(); i != fragmentBuffer.end(); ++i)
	{
		i->clear();
	}
}

static void testRasterizeTriangle()
{
#ifndef NDEBUG
	DEBUG_OUT << "Testing rasterizeTriangle()..." << std::endl;

	GLint const viewportX = 0;
	GLint const viewportY = 0;
	GLint const viewportWidth = 100;
	GLint const viewportHeight = viewportWidth;
	GLint const right = viewportWidth - 1;
	GLint const top = viewportHeight - 1;
	int totalErrorCount = 0;
	QImage image(viewportWidth, viewportHeight, QImage::Format_ARGB32);
	image.fill(0);

	DEBUG_OUT << "viewport: " << viewportX << ' ' << viewportY << ' ' << viewportWidth << ' ' << viewportHeight << std::endl;

	RasterizationSpans spans(viewportHeight);
	FragmentBuffer fragmentBuffer(viewportHeight * viewportWidth);

	int const rgba1 = 0xFFFF0000;
	int const rgba2 = 0x7F00FF00;

	std::vector< int > expectedRowSummary0;
	expectedRowSummary0.push_back(0);

	std::vector< int > expectedRowSummary1;
	expectedRowSummary1.push_back(0);
	expectedRowSummary1.push_back(rgba1);
	expectedRowSummary1.push_back(0);

	std::vector< int > expectedRowSummary2;
	expectedRowSummary2.push_back(0);
	expectedRowSummary2.push_back(rgba2);
	expectedRowSummary2.push_back(0);

	std::vector< int > expectedRowSummary3;
	expectedRowSummary3.push_back(0);
	expectedRowSummary3.push_back(rgba1);
	expectedRowSummary3.push_back(rgba2);
	expectedRowSummary3.push_back(0);

	std::vector< int > expectedRowSummary4;
	expectedRowSummary4.push_back(0);
	expectedRowSummary4.push_back(rgba2);
	expectedRowSummary4.push_back(rgba1);
	expectedRowSummary4.push_back(0);

	for (int i = 0; i < 2 * viewportWidth; ++i)
	{
		glm::vec4 v1(i, 0.0f, 0.0f, 1.0f);
		glm::vec4 v2(right - i, top, 0.0f, 1.0f);
		glm::vec4 v3(0.0f, top - i, 0.0f, 1.0f);
		glm::vec4 v4(right, i, 0.0f, 1.0f);

		if (viewportWidth <= i)
		{
			int const j = i - viewportWidth;
			v1 = glm::vec4(right, j, 0.0f, 1.0f);
			v2 = glm::vec4(0.0f, top - j, 0.0f, 1.0f);
			v3 = glm::vec4(j, 0.0f, 0.0f, 1.0f);
			v4 = glm::vec4(right - j, top, 0.0f, 1.0f);
		}

		int currentErrorCount = 0;

		clearStacks(fragmentBuffer);

		rasterizeTriangle(spans, viewportWidth, viewportHeight, v1, v2, v3, rgba1, fragmentBuffer);
		rasterizeTriangle(spans, viewportWidth, viewportHeight, v1, v4, v2, rgba2, fragmentBuffer);

		for (int y = 0; y < viewportHeight; ++y)
		{
			std::vector< int > actualRowSummary(1, 0);

			for (int x = 0; x < viewportWidth; ++x)
			{
				FragmentStack const & fragments = fragmentBuffer[y * viewportWidth + x];

				if (!fragments.empty())
				{
					if (fragments.size() != 1)
					{
						++currentErrorCount;

						DEBUG_OUT << "i: " << i << " y: " << y << " x: " << x << " badFragmentCount: " << fragments.size() << std::endl;
					}

					int const rgba = fragments[0].rgba();

					if (actualRowSummary.back() != rgba)
					{
						actualRowSummary.push_back(rgba);
					}
				}
				else
				{
					if (actualRowSummary.back() != 0)
					{
						actualRowSummary.push_back(0);
					}
				}
			}

			if (actualRowSummary.back() != 0)
			{
				actualRowSummary.push_back(0);
			}

			if (!(expectedRowSummary0 == actualRowSummary || expectedRowSummary1 == actualRowSummary ||
					expectedRowSummary2 == actualRowSummary || expectedRowSummary3 == actualRowSummary ||
					expectedRowSummary4 == actualRowSummary))
			{
				++currentErrorCount;

				using namespace Debug;

				DEBUG_OUT << "i: " << i << " y: " << y << " badRowSummary: " << actualRowSummary << std::endl;
			}
		}

		totalErrorCount += currentErrorCount;

		if (0 < currentErrorCount)
		{
			sortAndBlend(fragmentBuffer, image);

			std::stringstream fileName;
			fileName << "testRasterizeTriangle_" << i << ".png";

			QFile file(fileName.str().c_str());

			if (file.open(QIODevice::WriteOnly))
			{
				image.save(&file);
			}
		}
	}

	if (totalErrorCount == 0)
	{
		DEBUG_OUT << "rasterizeTriangle() seems OK" << std::endl;
	}
	else
	{
		DEBUG_OUT << "rasterizeTriangle() is defective: " << totalErrorCount << " error(s) found" << std::endl;
	}

	assert(totalErrorCount == 0);
#endif
}

} /* namespace VolumeExplorerTools */

} /* namespace CGoGN */

#endif /* SOFTWARE_FRAGMENT_SORTING_H */
