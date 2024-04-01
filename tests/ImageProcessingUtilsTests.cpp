#include "CppUnitTest.h"
#include "../src/ImageProcessingUtils/ImageProcessingUtils.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(AlgorithmTests)
{
public:
	TEST_METHOD(BGR2HSVTest)
	{
		cv::Mat src, dst;
		Algorithm::BGR2HSV(src, dst);
		Assert::IsTrue(dst.empty());
	}
};