/*
 * synthesizer.h
 *
 *  Created on: 12 maj 2019
 *      Author: piotr
 */

#pragma once
#include <memory>
#include <thread>
#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/video.hpp>
#include <opencv4/opencv2/objdetect/objdetect.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#include "utilities.h"
#include "slitScan.h"
#include "buffer.h"

const struct delayLine
{
  uint8_t delayRed = 100;
  uint8_t delayGreen = 100;
  uint8_t delayBlue = 100;
} delayLineParams;

class Synthesizer
{
public:
  Synthesizer(std::unique_ptr<cv::VideoCapture> input = nullptr, std::unique_ptr<cv::VideoWriter> output = nullptr);
  ~Synthesizer();
  void setInputStream(std::unique_ptr<cv::VideoCapture> input);
  void setOutputStream(std::unique_ptr<cv::VideoWriter> output);
  void setDelayMask(cv::Mat& delayMask);
  void startSynth();
  void stopSynth();
  bool isRunning();
  void setDelayRed(uint8_t delayRed);
  void setDelayGreen(uint8_t delayGreen);
  void setDelayBlue(uint8_t delayBlue);
  void setCurrentFactor(uint8_t gain);
  void setForgottenFactor(uint8_t gain);
  void setRedGain(uint8_t gain);
  void setGreenGain(uint8_t gain);
  void setBlueGain(uint8_t gain);
  void registerDisplay(std::function<void(cv::Mat&)> callback);

private:
  /*
   * Process data read from file and write it to file
   */
  void processLoopRt();

  /*
   * Process buffered data in loop
   */
  void processLoop();
  int processWorker(videoBuffer& buff, uint8_t chanNum, uint8_t& delayParam);

  std::unique_ptr<cv::VideoCapture> m_inputSt;
  std::unique_ptr<cv::VideoWriter> m_outputSt;
  size_t m_frameWidth;
  size_t m_frameHeight;
  size_t m_filterSize[3];
  int m_fps;
  cv::Mat m_frameIn;
  cv::Mat m_frameOut;
  std::shared_ptr<Buffer> m_inputBuff;
  std::shared_ptr<Buffer> m_outputBuff;
  std::thread m_loopTh;
  std::vector<cv::Mat> m_delayMask;
  bool m_procVideo;
  //life parameters
  std::shared_ptr<Params> m_rtParams;
  SlitScan m_process;
  std::function<void(cv::Mat&)> m_display;
};
