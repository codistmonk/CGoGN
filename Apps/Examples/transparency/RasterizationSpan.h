#ifndef RASTERIZATION_SPAN_H
#define RASTERIZATION_SPAN_H

namespace CGoGN
{

namespace Transparency
{

/**
 * Represents the pixels of a horizontal slice of a 2D triangle going from firstX to lastX (screen coordinates).
 */
class RasterizationSpan
{

	int m_firstX, m_lastX;

	// Barycentric coordinates in triangle
	float m_firstA, m_firstB, m_firstC;
	float m_lastA, m_lastB, m_lastC;

public:

	/**
	 * expands this if x is outside the current range [firstX() .. lastX()].
	 * a, b and c must be the barycentric coordinates of the triangle point corresponding to the fragment (x, y) (y is this ordinate).
	 * \param x Abscissa in screen coordinates
	 */
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

} /* namespace Transparency */

} /* namespace CGoGN */

#endif /* RASTERIZATION_SPAN_H */