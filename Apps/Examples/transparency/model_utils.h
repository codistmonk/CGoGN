#ifndef MODEL_UTILS_H
#define MODEL_UTILS_H

namespace CGoGN
{

namespace VolumeExplorerTools
{

template<typename PFP>
static void updateColorVBO(typename PFP::MAP& map, VolumeAttribute<typename PFP::VEC4> & colorPerXXX, const FunctorSelect& good,
		Algo::Render::GL2::ExplodeVolumeAlphaRender * const evr)
{
	VBODataPointer colors(evr->colors());

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

template<typename PFP>
static void explodeModel(typename PFP::MAP& map, VertexAttribute<typename PFP::VEC3> & vertexLocations, const FunctorSelect& good,
		float const volumeScale, float const faceScale,
		Algo::Render::GL2::ExplodeVolumeAlphaRender * const evr)
{
	VBODataPointer const colors(evr->colors());
	VBODataPointer vertices(evr->vertices());

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
				linerp<PFP>(linerp<PFP>(va, faceCenter, faceScale), volumeCenter, volumeScale);
				linerp<PFP>(linerp<PFP>(vb, faceCenter, faceScale), volumeCenter, volumeScale);
				linerp<PFP>(linerp<PFP>(vc, faceCenter, faceScale), volumeCenter, volumeScale);
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

template<typename PFP>
static int computeDepths(typename PFP::MAP& map, const FunctorSelect& good, std::vector<int> & depths)
{
	TraversorCell<typename PFP::MAP, PFP::MAP::FACE_OF_PARENT> modelFaces(map, good);

	int depth = 0;
	bool notDone = false;

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
			alpha *= pow(opacityGradient * 2.0, lastDepth - depth);
		}
		else
		{
			alpha *= pow((1.0 - opacityGradient) * 2.0, depth);
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

} /* namespace VolumeExplorerTools */

} /* namespace CGoGN */

#endif /* MODEL_UTILS_H */
