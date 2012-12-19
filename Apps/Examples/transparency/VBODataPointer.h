#ifndef VBO_DATA_POINTER_H
#define VBO_DATA_POINTER_H

namespace CGoGN
{

namespace GL2
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

} /* namespace CGoGN */

} /* namespace GL2 */

#endif /* VBO_DATA_POINTER_H */
