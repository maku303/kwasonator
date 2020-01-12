/*
 * synthesizer.cpp
 *
 *  Created on: 12 maj 2019
 *      Author: piotr
 */
#include <memory>
#include <thread>
#include <future>
#include <chrono>
#include <utility>
#include <string>
#include <vector>
#include "synthesizer.h"
#include "buffer.h"
#include "logger.h"


using namespace std;
using namespace cv;

Synthesizer::Synthesizer(std::unique_ptr<cv::VideoCapture> input, std::unique_ptr<cv::VideoWriter> output):
    m_inputSt{move(input)},
    m_outputSt{move(output)},
    m_frameWidth{},
    m_frameHeight{},
    m_filterSize{3, 3, 32},
    m_fps{},
    m_frameIn{},
    m_frameOut{},
    m_inputBuff{make_shared<Buffer>(1, 3)},
    m_outputBuff{make_shared<Buffer>(1, 3)},
    m_delayMask{},
    m_procVideo{false},
    m_rtParams{nullptr},
    m_process{}
{
  namedWindow("slitScan",1);
  m_process.bindInBuffers(m_inputBuff);
  m_process.bindOutBuffers(m_outputBuff);
  m_process.registerParams(m_rtParams);
}

Synthesizer::~Synthesizer()
{
}

void Synthesizer::setInputStream(std::unique_ptr<cv::VideoCapture> input)
{
  if (!isRunning())
  {
    m_inputSt = move(input);
    m_frameWidth = static_cast<int>(m_inputSt->get(CAP_PROP_FRAME_WIDTH));
    m_frameHeight = static_cast<int>(m_inputSt->get(CAP_PROP_FRAME_HEIGHT));
    m_fps = static_cast<int>(m_inputSt->get(CAP_PROP_FPS));
    cout<<m_frameWidth<<" "<<m_frameHeight<<" "<<m_fps<<endl;
    m_frameOut = Mat{static_cast<int>(m_frameHeight), static_cast<int>(m_frameWidth), CV_8UC3, cv::Scalar(0,0,0)};
  }
}

void Synthesizer::setOutputStream(std::unique_ptr<cv::VideoWriter> output)
{
  m_outputSt = move(output);
}

void Synthesizer::setDelayMask(cv::Mat& delayMask)
{
  m_process.updateMaskSrc(delayMask);
}

void Synthesizer::startSynth()
{
  if (!isRunning())
  {
    if (m_loopTh.joinable())
    {
      m_loopTh.join();
    }
    m_process.initParams(m_frameHeight, m_frameWidth, m_filterSize);
    m_process.processStart();
    m_loopTh = std::thread{&Synthesizer::processLoop, this};
    m_procVideo = true;
  }
}

void Synthesizer::stopSynth()
{
  if (isRunning())
  {
    m_procVideo = false;
    m_loopTh.join();
  }
}

void Synthesizer::setDelayRed(uint8_t delayRed)
{
  m_rtParams->delayRed = delayRed;
  m_rtParams->updated = true;
}

void Synthesizer::setDelayGreen(uint8_t delayGreen)
{
  m_rtParams->delayGreen = delayGreen;
  m_rtParams->updated = true;
}

void Synthesizer::setDelayBlue(uint8_t delayBlue)
{
  m_rtParams->delayBlue = delayBlue;
  m_rtParams->updated = true;
}

bool Synthesizer::isRunning()
{
  return m_procVideo;
}

void Synthesizer::setCurrentFactor(uint8_t gain)
{
  m_rtParams->currentFactor = gain;
  m_rtParams->updated = true;
}

void Synthesizer::setForgottenFactor(uint8_t gain)
{
  m_rtParams->forgottenFactor = gain;
  m_rtParams->updated = true;
}

void Synthesizer::setRedGain(uint8_t att)
{
  m_rtParams->blckoutRed = att;
  m_rtParams->updated = true;
}

void Synthesizer::setGreenGain(uint8_t att)
{
  m_rtParams->blckoutGreen = att;
  m_rtParams->updated = true;
}

void Synthesizer::setBlueGain(uint8_t att)
{
  m_rtParams->blckoutBlue = att;
  m_rtParams->updated = true;
}

/*
 * Intermediate method, with internal loop, no external module architecture
 */
void Synthesizer::processLoop()
{
  LOG("Synthesizer::processLoop() intro");
  bool allowNewFrame = true;
  Mat tempFrame;
  for(;;)
  {
    if (!m_procVideo) { break;}
    auto start = std::chrono::system_clock::now();

    if (allowNewFrame)
    {
      *m_inputSt >> m_frameIn;
      if (m_frameIn.empty()) {break;}
      if (!m_inputBuff->pushMultiChanFrame(m_frameIn))
      {
        allowNewFrame = false;
      }
      else allowNewFrame = true;
    }
    else
    {
      if (!m_inputBuff->pushMultiChanFrame(m_frameIn))
      {
        allowNewFrame = false;
      }
      else allowNewFrame = true;
    }

    Mat tempOutFrame;
    if (m_outputBuff->pullMultiChanFrame(m_frameOut))
    {
      m_display(m_frameOut);
      *m_outputSt<<m_frameOut;
    }
    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    LOG("ms per frame: "+to_string(elapsed.count()));
  }
  m_process.processStop();
  m_procVideo = false;
  LOG("m_procVideo  "+to_string(m_procVideo));
  LOG("Synthesizer::processLoop() finito");
}

void Synthesizer::registerDisplay(std::function<void(cv::Mat&)> callback)
{
    m_display = callback;
}

