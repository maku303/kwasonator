#include <string>
#include "logger.h"
#include "utilities.h"

using namespace std;
using namespace cv;

//move to GPU
void convolve(std::vector<vector<cv::Mat>>& frame, std::vector<vector<cv::Mat>>& filter)
{
  size_t frameDim3Size = frame[0].size();
  size_t frameDim1Size = static_cast<size_t>(frame[0][0].rows);
  size_t frameDim2Size = static_cast<size_t>(frame[0][0].cols);
  size_t filtDim3Size = filter[0].size();
  size_t filtDim1Size = static_cast<size_t>(filter[0][0].rows) >> 1;
  size_t filtDim2Size = static_cast<size_t>(filter[0][0].cols) >> 1;

  for (size_t i = filtDim1Size; i < frameDim1Size - filtDim1Size - 1; ++i)
  {
    for (size_t j = filtDim2Size; j < frameDim2Size - filtDim2Size - 1; ++j)
    {
      for (size_t k = 0; k < frameDim3Size; ++k)
      {
        cv::Mat framePartc1 = frame[0][k](cv::Rect(j - filtDim2Size, i - filtDim1Size, 3, 3));
        cv::Mat framePartc2 = frame[1][k](cv::Rect(i - filtDim1Size, j - filtDim2Size, 3, 3));
        cv::Mat framePartc3 = frame[2][k](cv::Rect(i - filtDim1Size, j - filtDim2Size, 3, 3));
        //WIP, TB implemented ...
      }
    }
  }


}

