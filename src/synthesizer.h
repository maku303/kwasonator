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
#include "circvidbuffer.h"

const struct delayLine
{
  uint8_t delayRed = 100;
  uint8_t delayGreen = 100;
  uint8_t delayBlue = 100;
} delayLineParams;


typedef union patchbay{
    struct connection
    {
        uint32_t camToFile : 1;
        uint32_t camToTransIn : 1;
        uint32_t camToCh1 : 1;
        uint32_t camToCh2 : 1;
        uint32_t camToCh3 : 1;
        uint32_t camToCh4 : 1;
        uint32_t camToScreen : 1;
        uint32_t fileToTransIn : 1;
        uint32_t fileToCh1 : 1;
        uint32_t fileToCh2 : 1;
        uint32_t fileToCh3 : 1;
        uint32_t fileToCh4 : 1;
        uint32_t fileToScreen : 1;
        uint32_t transOutToFile : 1;
        uint32_t transOutToCh1 : 1;
        uint32_t transOutToCh2 : 1;
        uint32_t transOutToCh3 : 1;
        uint32_t transOutToCh4 : 1;
        uint32_t transOutToScreen : 1;
        uint32_t patternToFile : 1;
        uint32_t patternToTransIn : 1;
        uint32_t patternToCh1 : 1;
        uint32_t patternToCh2 : 1;
        uint32_t patternToCh3 : 1;
        uint32_t patternToCh4 : 1;
        uint32_t patternToScreen : 1;
    } connect;
    struct source
    {
        uint32_t cam : 7;
        uint32_t file : 6;
        uint32_t transOut : 6;
        uint32_t pattern : 7;
    } device;
} patchbayLogic;

class Synthesizer
{
public:
  Synthesizer(std::unique_ptr<cv::VideoCapture> input = nullptr, std::unique_ptr<cv::VideoWriter> output = nullptr);
  ~Synthesizer();
  void setInputFileStream(std::unique_ptr<cv::VideoCapture> input);
  void setInputCamStream(std::unique_ptr<cv::VideoCapture> input);
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
  patchbayLogic& registerPatchbayLogic();
  bool isCamDevOpened();

private:
  /*
   * Process data read from file and write it to file
   */
  void processLoopRt();

  /*
   * Process buffered data in loop
   */
  void processLoop();

  std::unique_ptr<cv::VideoCapture> m_inputFileSt;
  std::unique_ptr<cv::VideoCapture> m_inputCamSt;
  std::unique_ptr<cv::VideoWriter> m_outputSt;
  size_t m_frameWidth;
  size_t m_frameHeight;
  size_t m_filterSize[3];
  int m_fps;
  cv::Mat m_frameFileIn;
  cv::Mat m_frameCamIn;
  cv::Mat m_frameOut;
  std::shared_ptr<Buffer> m_inputBuff;
  std::shared_ptr<Buffer> m_outputBuff;
  std::thread m_loopTh;
  std::vector<cv::Mat> m_delayMask;
  bool m_procVideo;
  //patchbay logic
  patchbayLogic m_patchLogic;
  //life parameters
  std::shared_ptr<Params> m_rtParams;
  SlitScan m_process;
  std::function<void(cv::Mat&)> m_display;
  CircVidBuffer m_ch1;
  CircVidBuffer m_ch2;
  CircVidBuffer m_ch3;
  CircVidBuffer m_ch4;
};
