#ifndef TRANSPARENCY_UTILS_H
#define TRANSPARENCY_UTILS_H

namespace CGoGN
{

namespace VolumeExplorerTools
{

/**
 * RAII wrapper for VBO data pointer.
 * VBO::lockPtr() is called in the constructor, and VBO::releasePtr() is called in the destructor.
 */
class VBODataPointer
{

	Utils::VBO * const m_vbo;

	float * const m_data;

public:

	VBODataPointer(Utils::VBO * const vbo): m_vbo(vbo), m_data(static_cast<float *>(vbo->lockPtr()))
	{
		// NOP
	}

	~VBODataPointer()
	{
		if (m_data)
		{
			m_vbo->releasePtr();
		}
	}

	unsigned int elementCount() const
	{
		return m_vbo->nbElts();
	}

	operator float *()
	{
		return m_data;
	}

	operator float const *() const
	{
		return m_data;
	}

};

/**
 * Linear interpolation: target = target * k + source * (1 - k).
 */
template<typename PFP>
static inline typename PFP::VEC3 & linerp(typename PFP::VEC3 & target, typename PFP::VEC3 const & source, float const k)
{
	target[0] = target[0] * k + source[0] * (1.0f - k);
	target[1] = target[1] * k + source[1] * (1.0f - k);
	target[2] = target[2] * k + source[2] * (1.0f - k);

	return target;
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
	glFlush(); DEBUG_GL;
	fbo->Unbind(); DEBUG_GL;
	glDrawBuffer(GL_BACK); DEBUG_GL;
}

/**
 * Simple class to retrieve OpenGL viewport information when the constructor is called.
 */
class Viewport
{

	GLint m_viewport[4];

public:

	Viewport()
	{
		glGetIntegerv(GL_VIEWPORT, m_viewport); DEBUG_GL;
	}

	GLint x() const
	{
		return m_viewport[0];
	}

	GLint y() const
	{
		return m_viewport[1];
	}

	GLint width() const
	{
		return m_viewport[2];
	}

	GLint height() const
	{
		return m_viewport[3];
	}

	operator GLint const *() const
	{
		return m_viewport;
	}

};

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

static inline std::ostream & operator<<(std::ostream & out, Viewport const & v)
{
	out << '[' << v.x() << ' ' << v.y() << ' ' << v.width() << ' ' << v.height() << ']';

	return out;
}

} /* namespace Debug */

} /* namespace VolumeExplorerTools */

} /* namespace CGoGN */

#endif /* TRANSPARENCY_UTILS_H */
