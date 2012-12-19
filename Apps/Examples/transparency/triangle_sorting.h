#ifndef TRIANGLE_SORTING_H
#define TRIANGLE_SORTING_H

#include "TriangleSortingComparator.h"
#include "VBODataPointer.h"

namespace CGoGN
{

namespace Transparency
{

/**
 * Sorts the triangles using data from evr's VBOs.
 * Upon successful completion, triangles is properly resized and contains the triangles in painter's algorithm order.
 * Each triangle is represented by 4 indices pointing to the volume center and the 3 vertices,
 * to be used in a geometry shader in accordance with the layout of evr->vertices().
 */
static void sortTriangles(Algo::Render::GL2::ExplodeVolumeAlphaRender const * const evr,
		std::vector<GLuint> & triangles, glm::vec4 const & viewpoint)
{
	static std::vector<unsigned int> permutation;
	unsigned int const n = evr->nbTris();

	permutation.resize(n);

	for (GLuint i = 0; i < n; ++i)
	{
		permutation[i] = i;
	}

	GL2::VBODataPointer const colors(evr->colors());
	GL2::VBODataPointer const vertices(evr->vertices());

	if (colors && vertices)
	{
		assert(colors.elementCount() == vertices.elementCount());

		std::sort(permutation.begin(), permutation.end(), TriangleSortingComparator(colors, vertices, colors.elementCount(), viewpoint));
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

} /* namespace Transparency */

} /* namespace CGoGN */

#endif /* TRIANGLE_SORTING_H */
