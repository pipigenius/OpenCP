#pragma once

#include "opencp.hpp"

#ifdef _DEBUG
#pragma comment(lib, "opencpd.lib")
#else
#pragma comment(lib, "opencp.lib")
#endif

void testAlphaBlend(cv::Mat& src1, cv::Mat& src2);
void testAlphaBlendMask(cv::Mat& src1, cv::Mat& src2);

void testCropZoom();
void testHistogram();
void testRGBHistogram();
void testRGBHistogram2();
void testAddNoise(cv::Mat& src);
void testTimer(cv::Mat& src);
void testPSNR(cv::Mat& ref);
void testLocalPSNR(cv::Mat& ref);
void guiConsoleTest();
void splitmergeTest(cv::Mat& src);

void guiDisparityPlaneFitSLICTest(cv::Mat& leftim, cv::Mat& rightim, cv::Mat& GT);
void fitPlaneTest();

void guiHazeRemoveTest();
void guiGuidedImageFilterTest();
void guiEdgePresevingFilterOpenCV(cv::Mat& src);
void guiWeightMapTest();
void guiCrossBasedLocalFilter(cv::Mat& src);
void guiStereoBMTest(cv::Mat& leftim, cv::Mat& rightim, const int numDisparities, const int disparity_min);
void guiStereoSGBMTest(cv::Mat& leftim, cv::Mat& rightim, const int numDisparities, const int disparity_min);

void guiSLICTest(cv::Mat& src);

void guiBilateralFilterTest(cv::Mat& src);
void guiSeparableBilateralFilterTest(cv::Mat& src);

void timeBirateralTest(cv::Mat& src);

void guiDualBilateralFilterTest(cv::Mat& src1, cv::Mat& src2);

void guiGausianFilterTest(cv::Mat& src_);

void guiRecursiveBilateralFilterTest(cv::Mat& src);
void guiRealtimeO1BilateralFilterTest(cv::Mat& src);
void getPSNRRealtimeO1BilateralFilterKodak();
void guiJointRealtimeO1BilateralFilterTest(cv::Mat& src_, cv::Mat& guide_);

void guiJointBirateralFilterTest(cv::Mat& src, cv::Mat& guide);

void guiGuidedFilterTest(cv::Mat& src);
void timeGuidedFilterTest(cv::Mat& src);

void guiDomainTransformFilterTest(cv::Mat& src);
void guiJointDomainTransformFilterTest(cv::Mat& src, cv::Mat& guide);

void guiCodingDistortionRemoveTest(cv::Mat& src);
void guiBinalyWeightedRangeFilterTest(cv::Mat& src);
void guiJointBinalyWeightedRangeFilterTest(cv::Mat& src, cv::Mat& guide);

void guiDomainTransformFilter(cv::Mat& src);

void guiNonLocalMeansTest(cv::Mat& src);
void guiSeparableNLMTest(cv::Mat& src);

void guiIterativeBackProjectionTest(cv::Mat& src);

//for application
void guiDetailEnhancement(cv::Mat& src);
void guiDenoiseTest(cv::Mat& src);


void guiViewSynthesis();

void guiJointNearestFilterTest(cv::Mat& src);

void fftTest(cv::Mat& src);

void guiHazeRemoveTest(cv::Mat& haze);

void qualityMetricsTest();

void guiCoherenceEnhancingShockFilter(cv::Mat& src, cv::Mat& dest);

void testPlot();

void testHistgram(cv::Mat& src);

void guiUpsampleTest(cv::Mat& src_);

void guiAnalysisCompare(cv::Mat& src1, cv::Mat& src2);
void imshowAnalysisCompare(cv::String winname, cv::Mat& src1, cv::Mat& src2);
void imshowAnalysis(cv::String winname, std::vector<cv::Mat>& s);
void imshowAnalysis(cv::String winname, cv::Mat& src);

enum DRAW_SIGNAL_CHANNEL
{
	B,
	G,
	R,
	Y
};
void drawSignalX(cv::Mat& src1, cv::Mat& src2, DRAW_SIGNAL_CHANNEL color, cv::Mat& dest, cv::Size outputImageSize, int line_height, int shiftx, int shiftvalue, int rangex, int rangevalue, int linetype = cp::Plot::LINE_LINEAR);// color 0:B, 1:G, 2:R, 3:Y
void drawSignalX(cv::InputArray src, DRAW_SIGNAL_CHANNEL color, cv::Mat& dest, cv::Size outputImageSize, int analysisLineHeight, int shiftx, int shiftvalue, int rangex, int rangevalue, int linetype = cp::Plot::LINE_LINEAR);// color 0:B, 1:G, 2:R, 3:Y

void drawSignalY(cv::Mat& src1, cv::Mat& src2, DRAW_SIGNAL_CHANNEL color, cv::Mat& dest, cv::Size size, int line_height, int shiftx, int shiftvalue, int rangex, int rangevalue, int linetype = cp::Plot::LINE_LINEAR);// color 0:B, 1:G, 2:R, 3:Y
void drawSignalY(cv::Mat& src, DRAW_SIGNAL_CHANNEL color, cv::Mat& dest, cv::Size size, int line_height, int shiftx, int shiftvalue, int rangex, int rangevalue, int linetype = cp::Plot::LINE_LINEAR);// color 0:B, 1:G, 2:R, 3:Y
void drawSignalY(std::vector<cv::Mat>& src, DRAW_SIGNAL_CHANNEL color, cv::Mat& dest, cv::Size size, int line_height, int shiftx, int shiftvalue, int rangex, int rangevalue, int linetype = cp::Plot::LINE_LINEAR);// color 0:B, 1:G, 2:R, 3:Y

void guiAnalysisImage(cv::InputArray src);

void guiColorCorrectionTest(cv::Mat& src, cv::Mat& ref);

void guiWeightedHistogramFilterTest(cv::Mat& src, cv::Mat& guide);


using namespace cv;
using namespace cp;