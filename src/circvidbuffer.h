#ifndef CIRCVIDBUFFER_H
#define CIRCVIDBUFFER_H
#include <vector>
#include <opencv4/opencv2/core/core.hpp>

class CircVidBuffer
{
public:
    CircVidBuffer();
    CircVidBuffer& operator >> (cv::Mat& image);
    CircVidBuffer& operator << (cv::Mat& image);
    void clearBuff();
private:
    std::vector<cv::Mat> m_buff;
    size_t m_ind;
};

#endif // CIRCVIDBUFFER_H
