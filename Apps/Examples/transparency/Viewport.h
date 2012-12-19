#ifndef VIEWPORT_H
#define VIEWPORT_H

namespace CGoGN
{

namespace GL1
{

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

} /* namespace CGoGN */

} /* namespace GL1 */

#endif /* VIEWPORT_H */
