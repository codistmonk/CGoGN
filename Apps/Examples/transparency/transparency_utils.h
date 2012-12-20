#ifndef TRANSPARENCY_UTILS_H
#define TRANSPARENCY_UTILS_H

#include "VBODataPointer.h"
#include "Viewport.h"

namespace CGoGN
{

namespace Transparency
{

/**
 * Linear interpolation: target = target * k + source * (1 - k).
 * \returns target
 */
template<typename PFP>
static inline typename PFP::VEC3 & linerp(typename PFP::VEC3 const & source, typename PFP::VEC3 & target, float const k)
{
	target[0] = target[0] * k + source[0] * (1.0f - k);
	target[1] = target[1] * k + source[1] * (1.0f - k);
	target[2] = target[2] * k + source[2] * (1.0f - k);

	return target;
}

/**
 * Linear interpolation.
 * \returns b * s + a * (1 - s)
 */
static inline float linerp(float const a, float const b, float const s)
{
	return b * s + a * (1.0f - s);
}

/**
 * Matlab's Jet color gradient: dark blue, blue, cyan, yellow, red, dark red.
 * Alpha component is 1.
 * Default color when x < 0 is (1, 1, 1, 1).
 * Clamps to dark red when 1 <= x.
 * \param x Interpolation parameter
 * \returns the interpolated jet color
 */
template<typename VEC4>
static VEC4 const jetColor4(float const x)
{
	static VEC4 const defaultColor = VEC4(1, 1, 1, 1);
	static VEC4 const colors[] = {
			VEC4(0.0, 0.0, 0.5, 1.0),
			VEC4(0.0, 0.0, 1.0, 1.0),
			VEC4(0.0, 1.0, 1.0, 1.0),
			VEC4(1.0, 1.0, 0.0, 1.0),
			VEC4(1.0, 0.0, 0.0, 1.0),
			VEC4(0.5, 0.0, 0.0, 1.0),
	};
	static unsigned int const n = sizeof(colors) / sizeof(VEC4);
	static unsigned int const lastColorIndex = n - 1;

	if (x < 0.0)
	{
		return defaultColor;
	}

	if (1.0 <= x)
	{
		return colors[lastColorIndex];
	}

	float const xn = x * lastColorIndex;
	int const i = static_cast<int>(xn);
	float const r = xn - i;

	return colors[i] * (1.0 - r) + colors[i + 1] * r;
}

/**
 * \returns x * x.
 */
static inline float square(float const x)
{
	return x * x;
}

/**
 * \returns x * x + y * y + z * z.
 */
static inline float squaredNorm(float const x, float const y, float const z)
{
	return square(x) + square(y) + square(z);
}

/**
 * Performs the following operations:
 *  1. check fbo
 *  2. bind
 *  3. clear with color (0, 0, 0, 0) and depth 1
 *  4. unbind
 */
static void bindClearUnbind(CGoGN::Utils::FBO * const fbo)
{
	fbo->CheckFBO(); DEBUG_GL;
	fbo->Bind(); DEBUG_GL;
	fbo->EnableColorAttachments(); DEBUG_GL;
	glClearColor(0, 0, 0, 0); DEBUG_GL;
	glClearDepth(1.0f); DEBUG_GL;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); DEBUG_GL;
	fbo->SafeUnbind(); DEBUG_GL;
}

/**
 * Initializes image's pixels from data retrieved using glReadPixels() and image's dimensions.
 */
static void readPixelsTo(QImage & image)
{
	int const width = image.width();
	int const height = image.height();
	std::vector<GLubyte> pixels(width * height * 4);
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]); DEBUG_GL;

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			QColor const color(
					pixels[(y * width + x) * 4 + 0],
					pixels[(y * width + x) * 4 + 1],
					pixels[(y * width + x) * 4 + 2]);
			image.setPixel(x, height - 1 - y, color.rgba());
		}
	}
}

namespace Debug
{

static inline std::ostream & operator<<(std::ostream & out, glm::vec4 const & v)
{
	out << '[' << v[0] << ' ' << v[1] << ' ' << v[2] << ' ' << v[3] << ']';

	return out;
}

static inline std::ostream & operator<<(std::ostream & out, QPointF const & p)
{
	out << '[' << p.x() << ' ' << p.y() << ']';

	return out;
}

static inline std::ostream & operator<<(std::ostream & out, std::vector< int > const & v)
{
	out << '[';

	if (!v.empty())
	{
		std::vector< int >::const_iterator i = v.begin();
		out << *i;
		++i;

		while (i != v.end())
		{
			out << ' ' << *i;
			++i;
		}
	}

	out << ']';

	return out;
}

static inline std::ostream & operator<<(std::ostream & out, GL1::Viewport const & v)
{
	out << '[' << v.x() << ' ' << v.y() << ' ' << v.width() << ' ' << v.height() << ']';

	return out;
}

} /* namespace Debug */

} /* namespace Transparency */

} /* namespace CGoGN */

#endif /* TRANSPARENCY_UTILS_H */
