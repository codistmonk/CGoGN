#ifndef TRIANGLE_SORTING_H
#define TRIANGLE_SORTING_H

namespace CGoGN
{

namespace VolumeExplorerTools
{

/**
 * Comparator used to sort triangle by decreasing distance from viewpoint.
 * To compute the distance of a triangle, a reference point is taken halfway from its center to the volume center;
 * this avoids visual artifacts due to overlapping triangles in adjacent volumes.
 */
class Comparator
{
	static std::vector<float> & distances()
	{
		static std::vector<float> result;

		return result;
	}
public:
	Comparator(float const * const colorBufferWithFaceCenters, float const * const vertexBufferWithVolumeCenters,
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

/**
 * Sorts the triangles using data from evr's VBOs.
 * Upon successful completion, triangles is properly resized and contains the triangles in painter's algorithm order.
 * Each triangle is represented by 4 indices pointing to the volume center and the 3 vertices,
 * to be used in a geometry shader in accordance with the layout of evr->vertices().
 */
static void sortData(Algo::Render::GL2::ExplodeVolumeAlphaRender const * const evr,
		std::vector<GLuint> & triangles, glm::vec4 const & viewpoint)
{
	static std::vector<unsigned int> permutation;
	unsigned int const n = evr->nbTris();

	permutation.resize(n);

	for (GLuint i = 0; i < n; ++i)
	{
		permutation[i] = i;
	}

	VBODataPointer const colors(evr->colors());
	VBODataPointer const vertices(evr->vertices());

	if (colors && vertices)
	{
		assert(colors.elementCount() == vertices.elementCount());

		std::sort(permutation.begin(), permutation.end(), Comparator(colors, vertices, colors.elementCount(), viewpoint));
	}

	triangles.resize(n * 4);

	for (GLuint i = 0; i < n * 4; i += 4)
	{
		for (int j = 0; j < 4; ++j)
		{
			triangles[i + j] = permutation[i / 4] * 4 + j;
		}
	}
}

} /* namespace VolumeExplorerTools */

} /* namespace CGoGN */

#endif /* TRIANGLE_SORTING_H */
