#ifndef SOFTWARE_FRAGMENT_SORTING_TEST_H
#define SOFTWARE_FRAGMENT_SORTING_TEST_H

#include "software_fragment_sorting.h"

namespace CGoGN
{

namespace Transparency
{

static inline void clearPixelFragments(FragmentBuffer & fragmentBuffer)
{
	for (FragmentBuffer::iterator i = fragmentBuffer.begin(); i != fragmentBuffer.end(); ++i)
	{
		i->clear();
	}
}

/**
 * Tests that the seam between 2 triangles is rendered properly (no hole, no overlapping):
 *  * a square made of 2 adjacent triangles is rendered and scale-rotated so that its corners always touch a border of the viewport;
 *  * for each angle, each rendered fragment buffer line is scanned looking for holes or multiple fragments by pixels
 *    (a correct line has the spanning pattern 0?(1|12|21|2)0? where 1 (resp 2) stands for triangle 1 (resp triangle 2),
 *     and 0 is the background).
 */
static void testRasterizeTriangle()
{
#ifndef NDEBUG
	DEBUG_OUT << "Testing rasterizeTriangle()..." << std::endl;

	GLint const viewportX = 0;
	GLint const viewportY = 0;
	GLint const viewportWidth = 100;
	GLint const viewportHeight = viewportWidth;
	GLint const right = viewportWidth - 1;
	GLint const top = viewportHeight - 1;
	int totalErrorCount = 0;
	QImage image(viewportWidth, viewportHeight, QImage::Format_ARGB32);
	image.fill(0);

	DEBUG_OUT << "viewport: " << viewportX << ' ' << viewportY << ' ' << viewportWidth << ' ' << viewportHeight << std::endl;

	RasterizationSpans spans(viewportHeight);
	FragmentBuffer fragmentBuffer(viewportHeight * viewportWidth);

	int const rgba1 = 0xFFFF0000;
	int const rgba2 = 0x7F00FF00;

	std::vector< int > expectedRowPattern0;
	expectedRowPattern0.push_back(0);

	std::vector< int > expectedRowPattern1;
	expectedRowPattern1.push_back(0);
	expectedRowPattern1.push_back(rgba1);
	expectedRowPattern1.push_back(0);

	std::vector< int > expectedRowPattern2;
	expectedRowPattern2.push_back(0);
	expectedRowPattern2.push_back(rgba2);
	expectedRowPattern2.push_back(0);

	std::vector< int > expectedRowPattern3;
	expectedRowPattern3.push_back(0);
	expectedRowPattern3.push_back(rgba1);
	expectedRowPattern3.push_back(rgba2);
	expectedRowPattern3.push_back(0);

	std::vector< int > expectedRowPattern4;
	expectedRowPattern4.push_back(0);
	expectedRowPattern4.push_back(rgba2);
	expectedRowPattern4.push_back(rgba1);
	expectedRowPattern4.push_back(0);

	for (int i = 0; i < 2 * viewportWidth; ++i)
	{
		glm::vec4 v1(i, 0.0f, 0.0f, 1.0f);
		glm::vec4 v2(right - i, top, 0.0f, 1.0f);
		glm::vec4 v3(0.0f, top - i, 0.0f, 1.0f);
		glm::vec4 v4(right, i, 0.0f, 1.0f);

		if (viewportWidth <= i)
		{
			int const j = i - viewportWidth;
			v1 = glm::vec4(right, j, 0.0f, 1.0f);
			v2 = glm::vec4(0.0f, top - j, 0.0f, 1.0f);
			v3 = glm::vec4(j, 0.0f, 0.0f, 1.0f);
			v4 = glm::vec4(right - j, top, 0.0f, 1.0f);
		}

		int currentErrorCount = 0;

		clearPixelFragments(fragmentBuffer);

		rasterizeTriangle(spans, viewportWidth, viewportHeight, v1, v2, v3, rgba1, fragmentBuffer);
		rasterizeTriangle(spans, viewportWidth, viewportHeight, v1, v4, v2, rgba2, fragmentBuffer);

		for (int y = 0; y < viewportHeight; ++y)
		{
			std::vector< int > actualRowPattern(1, 0);

			for (int x = 0; x < viewportWidth; ++x)
			{
				PixelFragments const & fragments = fragmentBuffer[y * viewportWidth + x];

				if (!fragments.empty())
				{
					if (fragments.size() != 1)
					{
						++currentErrorCount;

						DEBUG_OUT << "i: " << i << " y: " << y << " x: " << x << " badFragmentCount: " << fragments.size() << std::endl;
					}

					int const rgba = fragments[0].rgba();

					if (actualRowPattern.back() != rgba)
					{
						actualRowPattern.push_back(rgba);
					}
				}
				else
				{
					if (actualRowPattern.back() != 0)
					{
						actualRowPattern.push_back(0);
					}
				}
			}

			if (actualRowPattern.back() != 0)
			{
				actualRowPattern.push_back(0);
			}

			if (!(expectedRowPattern0 == actualRowPattern || expectedRowPattern1 == actualRowPattern ||
					expectedRowPattern2 == actualRowPattern || expectedRowPattern3 == actualRowPattern ||
					expectedRowPattern4 == actualRowPattern))
			{
				++currentErrorCount;

				using namespace Debug;

				DEBUG_OUT << "i: " << i << " y: " << y << " badRowPattern: " << actualRowPattern << std::endl;
			}
		}

		totalErrorCount += currentErrorCount;

		if (0 < currentErrorCount)
		{
			sortAndBlend(fragmentBuffer, image);

			std::stringstream fileName;
			fileName << "testRasterizeTriangle_" << i << ".png";

			QFile file(fileName.str().c_str());

			if (file.open(QIODevice::WriteOnly))
			{
				image.save(&file);
			}
		}
	}

	if (totalErrorCount == 0)
	{
		DEBUG_OUT << "rasterizeTriangle() seems OK" << std::endl;
	}
	else
	{
		DEBUG_OUT << "rasterizeTriangle() is defective: " << totalErrorCount << " error(s) found" << std::endl;
	}

	assert(totalErrorCount == 0);
#endif
}

} /* namespace Transparency */

} /* namespace CGoGN */

#endif /* SOFTWARE_FRAGMENT_SORTING_TEST_H */
