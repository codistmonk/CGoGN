#ifndef PIXEL_FRAGMENT_H
#define PIXEL_FRAGMENT_H

namespace CGoGN
{

namespace Transparency
{

/**
 * Fragment sorting datum.
 */
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

} /* namespace Transparency */

} /* namespace CGoGN */

#endif /* PIXEL_FRAGMENT_H */
