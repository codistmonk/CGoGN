#ifndef MODEL_UTILS_H
#define MODEL_UTILS_H

#include "VBODataPointer.h"

namespace CGoGN
{

namespace Transparency
{

/**
 * Sends the color data from colorPerXXX to the corresponding VBO (evr->colors()).
 * The VBO must already be allocated with the correct size.
 */
template<typename PFP>
static void updateColorVBO(typename PFP::MAP& map, VolumeAttribute<typename PFP::VEC4> & colorPerXXX, const FunctorSelect& good,
		Algo::Render::GL2::ExplodeVolumeAlphaRender * const evr)
{
	GL2::VBODataPointer colors(evr->colors());

	if (colors)
	{
		typedef typename PFP::VEC4 VEC4;

		TraversorCell<typename PFP::MAP, PFP::MAP::FACE_OF_PARENT> modelFaces(map, good);

		int faceIndex = 0;

		for (Dart d = modelFaces.begin(); d != modelFaces.end(); d = modelFaces.next())
		{
			Dart a = d;
			Dart b = map.phi1(a);
			Dart c = map.phi1(b);
			do
			{
				memcpy(&colors[faceIndex * 4 * 4 + 1 * 4], &colorPerXXX[d], sizeof(VEC4));
				memcpy(&colors[faceIndex * 4 * 4 + 2 * 4], &colorPerXXX[b], sizeof(VEC4));
				memcpy(&colors[faceIndex * 4 * 4 + 3 * 4], &colorPerXXX[c], sizeof(VEC4));

				++faceIndex;
				b = c;
				c = map.phi1(b);
			} while (c != d);
		}
	}
}

/**
 * Performs an 'explode' operation, ie for each triangle vertex, vertex_location = linerp(vertex_location, faceScale), volumeCenter, volumeScale).
 * Values are computed from vertexLocations (which is left untouched) and sent to the VBOs evr->colors() and evr->vertices().
 * Face and volume centers must already be computed and stored in the VBOs.
 * Each face is represented by:
 *  * in the color VBO : face_center_xyzw, vertex1_rgba, vertex2_rgba, vertex3_rgba
 *  * in the vertex VBO : volume_center_xyz, vertex1_xyz, vertex2_xyz, vertex3_xyz.
 * The vertexX_xyz are updated.
 */
template<typename PFP>
static void explodeModel(typename PFP::MAP& map, VertexAttribute<typename PFP::VEC3> & vertexLocations, const FunctorSelect& good,
		float const volumeScale, float const faceScale,
		Algo::Render::GL2::ExplodeVolumeAlphaRender * const evr)
{
	GL2::VBODataPointer const colors(evr->colors());
	GL2::VBODataPointer vertices(evr->vertices());

	if (colors && vertices)
	{
		assert(colors.elementCount() == vertices.elementCount());

		typedef typename PFP::VEC3 VEC3;

		TraversorCell<typename PFP::MAP, PFP::MAP::FACE_OF_PARENT> modelFaces(map, good);
		int faceIndex = 0;

		for (Dart d = modelFaces.begin(); d != modelFaces.end(); d = modelFaces.next())
		{
			VEC3 const volumeCenter = VEC3(vertices[faceIndex * 4 * 3 + 0], vertices[faceIndex * 4 * 3 + 1], vertices[faceIndex * 4 * 3 + 2]);
			VEC3 const faceCenter = VEC3(colors[faceIndex * 4 * 4 + 0], colors[faceIndex * 4 * 4 + 1], colors[faceIndex * 4 * 4 + 2]);
			Dart a = d;
			Dart b = map.phi1(a);
			Dart c = map.phi1(b);
			do
			{
				VEC3 va = vertexLocations[a];
				VEC3 vb = vertexLocations[b];
				VEC3 vc = vertexLocations[c];
				linerp<PFP>(volumeCenter, linerp<PFP>(faceCenter, va, faceScale), volumeScale);
				linerp<PFP>(volumeCenter, linerp<PFP>(faceCenter, vb, faceScale), volumeScale);
				linerp<PFP>(volumeCenter, linerp<PFP>(faceCenter, vc, faceScale), volumeScale);
				memcpy(&vertices[faceIndex * 4 * 3 + 1 * 3], &va, sizeof(VEC3));
				memcpy(&vertices[faceIndex * 4 * 3 + 2 * 3], &vb, sizeof(VEC3));
				memcpy(&vertices[faceIndex * 4 * 3 + 3 * 3], &vc, sizeof(VEC3));

				++faceIndex;
				b = c;
				c = map.phi1(b);
			} while (c != d);
		}
	}
}

/**
 * Computes the topological depth of each dart.
 * Surface volumes have depth 0.
 * Inside volumes adjacent to surface volumes have depth 1, and so on.
 * The int vector depths must already be sized so that the depth of each dart can be stored in depths[dart.label()].
 * The int vector depths must already be initialized with -1 for each dart.
 * \returns the largest depth found (minimum 0).
 */
template<typename PFP>
static int computeDepths(typename PFP::MAP& map, const FunctorSelect& good, std::vector<int> & depths)
{
	TraversorCell<typename PFP::MAP, PFP::MAP::FACE_OF_PARENT> modelFaces(map, good);

	int depth = 0;
	bool notDone = false;

	/* initialize_depths_of_surface_darts: */
	for (Dart d = modelFaces.begin(); d != modelFaces.end(); d = modelFaces.next())
	{
		if (map.isBoundaryFace(d))
		{
			notDone = true;
			Traversor3WF<typename PFP::MAP> cellFaces(map, d);

			for (Dart e = cellFaces.begin(); e != cellFaces.end(); e = cellFaces.next())
			{
				Traversor3FE<typename PFP::MAP> faceEdges(map, e);

				for (Dart f = faceEdges.begin(); f != faceEdges.end(); f = faceEdges.next())
				{
					depths[f.label()] = depth;
				}
			}
		}
	}

	++depth;

	/* propagate_depths_from_surface: */
	while (notDone)
	{
		notDone = false;

		for (Dart d = modelFaces.begin(); d != modelFaces.end(); d = modelFaces.next())
		{
			if (depths[d.label()] == -1 && depths[map.phi3(d).label()] == depth - 1)
			{
				notDone = true;
				Traversor3WF<typename PFP::MAP> cellFaces(map, d);

				for (Dart e = cellFaces.begin(); e != cellFaces.end(); e = cellFaces.next())
				{
					Traversor3FE<typename PFP::MAP> faceEdges(map, e);

					for (Dart f = faceEdges.begin(); f != faceEdges.end(); f = faceEdges.next())
					{
						depths[f.label()] = depth;
					}
				}
			}
		}

		if (notDone)
		{
			++depth;
		}
	}

	return depth - 1;
}

/**
 * Applies a Jet gradient to the color VBO; each volume gets a uniform color according to its depth (deepest: dark red, shallowest: dark blue).
 * Alpha values are computed from depth values with a nonlinear mapping; opacityGradient controls the shape of the mapping curve:
 *  * opacityGradient < 0.5 : deep volumes are more opaque.
 *  * 0.5 <= opacityGradient : shallow volumes are more opaque.
 *  
 *  \param opacity The maximum opacity (alpha value) between 0 and 1
 *  \param opacityGradient The opacity curve control between 0 and 1
 */
template<typename PFP>
static void computeColorsUsingDepths(typename PFP::MAP & map, VolumeAttribute<typename PFP::VEC4> & colorPerXXX,
		float const opacity, float const opacityGradient, FunctorSelect const & good,
		std::vector<int> const & depths, int const lastDepth)
{
	TraversorCell<typename PFP::MAP, PFP::MAP::FACE_OF_PARENT> faces(map, good);

	for (Dart d = faces.begin(); d != faces.end(); d = faces.next())
	{
		int const depth = depths[d.label()];
		float const normalizedDepth = lastDepth == 0 ? 1.0 : static_cast<float>(depth) / lastDepth;
		float alpha = opacity;

		if (opacityGradient < 0.5)
		{
			alpha = 1.0 - normalizedDepth <= opacityGradient * 2.0f ? opacity : opacity * pow(opacityGradient * 2.0, lastDepth - depth);
		}
		else
		{
			alpha = normalizedDepth <= (1.0 - opacityGradient) * 2.0f ? opacity : opacity * pow((1.0 - opacityGradient) * 2.0, depth);
		}

		Dart a = d;
		Dart b = map.phi1(a);
		Dart c = map.phi1(b);
		colorPerXXX[a] = jetColor4<typename PFP::VEC4>(normalizedDepth);
		colorPerXXX[a][3] = alpha;
		colorPerXXX[b] = jetColor4<typename PFP::VEC4>(normalizedDepth);
		colorPerXXX[b][3] = alpha;
		colorPerXXX[c] = jetColor4<typename PFP::VEC4>(normalizedDepth);
		colorPerXXX[c][3] = alpha;
	}
}

/**
 * Updated position so that the resulting centroid is (0, 0, 0).
 */
template<typename PFP>
static void centerModel(typename PFP::MAP & myMap, VertexAttribute<typename PFP::VEC3> & position)
{
	typename PFP::VEC3 centroid(0, 0, 0);
	float count = 0;

	/*compute_centroid:*/
	{
		TraversorCell<typename PFP::MAP, VOLUME> tra(myMap);

		for (Dart d = tra.begin(); d != tra.end(); d = tra.next())
		{
			centroid += position[d];
			++count;
		}

		centroid /= count;
	}

	/*update_position:*/
	{
		TraversorCell<typename PFP::MAP, VERTEX> tra(myMap);

		for (Dart d = tra.begin(); d != tra.end(); d = tra.next())
		{
			position[d] -= centroid;
		}
	}
}

} /* namespace Transparency */

} /* namespace CGoGN */

#endif /* MODEL_UTILS_H */
