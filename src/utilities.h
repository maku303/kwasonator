#pragma once

#include <vector>
#include <iostream>
#include <memory>
#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/video.hpp>
#include <opencv4/opencv2/objdetect/objdetect.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>

void convolve(std::vector<std::vector<cv::Mat>>& frame, std::vector<std::vector<cv::Mat>>& filter);

