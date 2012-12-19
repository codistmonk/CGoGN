#ifndef TRIANGLE_SORTING_COMPARATOR_H
#define TRIANGLE_SORTING_COMPARATOR_H

namespace CGoGN
{

namespace Transparency
{

/**
 * Comparator used to sort triangle by decreasing distance from viewpoint.
 * To compute the distance of a triangle, a reference point is taken halfway from its center to the volume center;
 * this avoids visual artifacts due to overlapping triangles in adjacent volumes.
 */
class TriangleSortingComparator
{

	static std::vector<float> & distances()
	{
		static std::vector<float> result;

		return result;
	}

public:

	TriangleSortingComparator(float const * const colorBufferWithFaceCenters, float const * const vertexBufferWithVolumeCenters,
			unsigned int const elementCount, glm::vec4 const & viewpoint)
	{
		distances().resize(elementCount / 4);

		for (unsigned int i = 0; i < distances().size(); ++i)
		{
			distances()[i] = squaredNorm(
				(colorBufferWithFaceCenters[4 * 4 * i + 0] + vertexBufferWithVolumeCenters[4 * 3 * i + 0]) / 2 - viewpoint.x,
				(colorBufferWithFaceCenters[4 * 4 * i + 1] + vertexBufferWithVolumeCenters[4 * 3 * i + 1]) / 2 - viewpoint.y,
				(colorBufferWithFaceCenters[4 * 4 * i + 2] + vertexBufferWithVolumeCenters[4 * 3 * i + 2]) / 2 - viewpoint.z);
		}
	}

	bool operator() (unsigned int const i, unsigned int const j) const
	{
		return distances()[i] > distances()[j];
	}

};

} /* namespace Transparency */

} /* namespace CGoGN */

#endif /* TRIANGLE_SORTING_COMPARATOR_H */
