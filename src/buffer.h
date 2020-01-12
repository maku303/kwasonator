#ifndef BUFFER_H
#define BUFFER_H
#include <iostream>
#include <opencv4/opencv2/core/core.hpp>

class Buffer
{
  enum class halfState{notReady, ready};
public:
  /*
   * size - per one half
  */
  Buffer(size_t buffSize = 1, size_t chanSize = 3);
  ~Buffer();
  /*
   * Returns pointer to buffer element to which will be written or null if to available
  */
  bool pushFrame(cv::Mat& frame, size_t chanNum, bool firstHalf);
  bool pullFrame(cv::Mat& frame, size_t chanNum, bool firstHalf);
  bool pushMultiChanFrame(cv::Mat& frame);
  bool pushMultiChanFrame(std::vector<cv::Mat>& frame);
  bool pullMultiChanFrame(cv::Mat& frame);
  bool pullMultiChanFrame(std::vector<cv::Mat>& frame);
  bool isHalfNotReady(std::vector<halfState>& buffState);
  bool isHalfReady(std::vector<halfState>& buffState);
  bool isFirstActiveHalf();
  void switchHalf();

private:
  size_t m_buffSize;
  size_t m_chanSize;
  std::vector<std::vector<cv::Mat>> m_buff1half;
  std::vector<std::vector<cv::Mat>> m_buff2half;
  std::vector<halfState> m_buff1State;
  std::vector<halfState> m_buff2State;
  std::vector<size_t> m_buff1ReadPtrIdx;
  std::vector<size_t> m_buff2ReadPtrIdx;
  std::vector<size_t> m_buff1WritePtrIdx;
  std::vector<size_t> m_buff2WritePtrIdx;
  //false first, true second
  volatile bool m_activeHalf;
};

#endif // BUFFER_H
