#ifndef VOLUME_EXPLORER_TOOLS_H
#define VOLUME_EXPLORER_TOOLS_H

#include <vector>

namespace CGoGN
{

namespace VolumeExplorerTools
{

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

static inline float square(float const x)
{
	return x * x;
}

static inline float squaredNorm(float const x, float const y, float const z)
{
	return square(x) + square(y) + square(z);
}

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

static void sortData(Algo::Render::GL2::ExplodeVolumeAlphaRender const * const evr, std::vector<unsigned int> & permutation,
		std::vector<GLuint> & triangles, glm::vec4 const & viewpoint)
{
	unsigned int const n = evr->nbTris();

	permutation.resize(n);

	for (GLuint i = 0; i < n; ++i)
	{
		permutation[i] = i;
	}

	Utils::VBO const * const colorVBO = evr->colors();
	Utils::VBO const * const vertexVBO = evr->vertices();

	float const * const colors = static_cast<float const *>(colorVBO->lockPtr());
	float const * const vertices = static_cast<float const *>(vertexVBO->lockPtr());

	if (colors && vertices)
	{
		assert(colorVBO->nbElts() == vertexVBO->nbElts());

		std::sort(permutation.begin(), permutation.end(), Comparator(colors, vertices, colorVBO->nbElts(), viewpoint));
	}

	if (colors)
	{
		colorVBO->releasePtr();
	}

	if (vertices)
	{
		vertexVBO->releasePtr();
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

} // namespace VolumeExplorerTools

} // namespace CGoGN

#endif
