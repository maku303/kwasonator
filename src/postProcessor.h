/*
 * slitScan.h
 *
 *  Created on: 27 maj 2019
 *      Author: piotr
 */

#pragma once

#include <thread>
#include <vector>
#include <memory>
#include <atomic>
#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/video.hpp>
#include<opencv4/opencv2/objdetect/objdetect.hpp>
#include<opencv4/opencv2/imgproc/imgproc.hpp>
#include <armadillo>

typedef arma::Cube<std::complex<float>> cxCubeMat;
typedef arma::Cube<float> CubeMat;

class PostProcessor
{
  enum class bufferStat {notReady, ready, busy, processed};
public:
  PostProcessor(size_t height, size_t width, size_t timeDelay);
  ~PostProcessor();
  void processStart();
  void processStop();
  void process();
  void processFft(bool firstHalf);
  void transformCxDomain(cxCubeMat& procCxMat, cxCubeMat& transformCxMat);
  void addFrame(const cv::Mat& frame);
  std::shared_ptr<cv::Mat> pullFrame();
  void onNewOutputFrame();
  /*
   * creates monochromatic object for tests data and fills fft buffer
   */
  void testObject();
  bool isBufferHungry();

private:
  std::vector<std::shared_ptr<std::thread>> m_proc;
  size_t m_height;
  size_t m_width;
  size_t m_timeDelay;
  int m_cxBytesCnt;
  bool m_procBusy;
  bufferStat m_buff1Stat;
  bufferStat m_buff2Stat;
  int m_framesPerHalfBuff;
  float* m_buffer1;
  float* m_buffer2;
  bool m_buffer1empty;
  bool m_buffer2empty;
  fftwf_complex* m_bufferCx1;
  fftwf_complex* m_bufferCx2;
  fftwf_plan m_planForw1;
  fftwf_plan m_planBack1;
  fftwf_plan m_planForw2;
  fftwf_plan m_planBack2;
  cxCubeMat m_processCxMat;
  cxCubeMat m_transformCxMat;
  CubeMat m_processBuffMat;
  size_t m_pullCounter;
  size_t m_newFrameCounter;
  cv::Mat m_out;
};
