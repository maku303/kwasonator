#pragma once

#include <vector>
#include <iostream>
#include <memory>
#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/video.hpp>
#include <opencv4/opencv2/objdetect/objdetect.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>


void slitScanTransform(const std::vector<cv::Mat*> inputBuff, cv::Mat& output, int delayH, int delayV);
int slitScanTransformElem(const std::vector<cv::Mat*> inputBuff, cv::Mat& output, cv::Mat& delayMask, uint8_t delayGain);
int slitScanTransformElem2(cv::Mat& inputBuff, cv::Mat& output, cv::Mat& delayMask, uint8_t delayGain);
void transformVertical();
void transformHorizontal();

class videoBuffer {
public:
	videoBuffer(int size = 100, cv::Size vidSize = {1920, 1080});
	~videoBuffer();
	void addNewElement(cv::Mat& el);
	std::vector<cv::Mat*> getBuffer();

	uint64_t m_buffSize;
	cv::Size m_vidSize;
	std::vector<cv::Mat> m_buff;
	size_t m_count;
	size_t m_pos;
};

void convolve(std::vector<std::vector<cv::Mat>>& frame, std::vector<std::vector<cv::Mat>>& filter);

