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
    m_inputFileSt{move(input)},
    m_outputSt{move(output)},
    m_frameWidth{1920},
    m_frameHeight{1080},
    m_filterSize{3, 3, 32},
    m_fps{25},
    m_frameFileIn{},
    m_frameCamIn{},
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

void Synthesizer::setInputFileStream(std::unique_ptr<cv::VideoCapture> input)
{
  if (!isRunning())
  {
    m_inputFileSt = move(input);
    m_frameWidth = static_cast<int>(m_inputFileSt->get(CAP_PROP_FRAME_WIDTH));
    m_frameHeight = static_cast<int>(m_inputFileSt->get(CAP_PROP_FRAME_HEIGHT));
    m_fps = static_cast<int>(m_inputFileSt->get(CAP_PROP_FPS));
    cout<<m_frameWidth<<" "<<m_frameHeight<<" "<<m_fps<<endl;
    m_frameOut = Mat{static_cast<int>(m_frameHeight), static_cast<int>(m_frameWidth), CV_8UC3, cv::Scalar(0,0,0)};
  }
}

void Synthesizer::setInputCamStream(std::unique_ptr<cv::VideoCapture> input)
{
    m_inputCamSt = move(input);
    m_frameWidth = static_cast<int>(m_inputCamSt->get(CAP_PROP_FRAME_WIDTH));
    m_frameHeight = static_cast<int>(m_inputCamSt->get(CAP_PROP_FRAME_HEIGHT));
    m_fps = static_cast<int>(m_inputCamSt->get(CAP_PROP_FPS));
    cout<<m_frameWidth<<" "<<m_frameHeight<<" "<<m_fps<<endl;
    m_frameOut = Mat{static_cast<int>(m_frameHeight), static_cast<int>(m_frameWidth), CV_8UC3, cv::Scalar(0,0,0)};
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

patchbayLogic& Synthesizer::registerPatchbayLogic()
{
  return m_patchLogic;
}


void Synthesizer::processLoop()
{
  LOG("intro");
  bool allowNewFrame = true;
  Mat tempFrame;
  Mat dstFile, dstTransIn, dstCh1, dstCh2, dstCh3, dstCh4;
  for(;;)
  {
    if (!m_procVideo) { break;}


    if (allowNewFrame)
    {
      auto start = std::chrono::system_clock::now();
      if (isCamDevOpened() && (m_patchLogic.device.cam))
      {
        cv::Mat temp;
        *m_inputCamSt >> temp;
        Size sz{static_cast<int>(m_frameWidth), static_cast<int>(m_frameHeight)};
        resize(temp, m_frameCamIn, sz, 0, 0);
        if (m_patchLogic.connect.camToFile)
        {
          dstFile = m_frameCamIn;
        }
        if (m_patchLogic.connect.camToTransIn)
        {
          dstTransIn = m_frameCamIn;
        }
        if (m_patchLogic.connect.camToCh1)
        {
          dstCh1 = m_frameCamIn;
        }
        if (m_patchLogic.connect.camToCh2)
        {
          dstCh2 = m_frameCamIn;
        }
        if (m_patchLogic.connect.camToCh3)
        {
          dstCh3 = m_frameCamIn;
        }
        if (m_patchLogic.connect.camToCh4)
        {
          dstCh4 = m_frameCamIn;
        }
        if (m_patchLogic.connect.camToScreen)
        {
           m_display(m_frameCamIn);
        }
      }
      auto end = std::chrono::system_clock::now();
      auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
      cout<<"ms per frame cam: "+to_string(elapsed.count())<<endl;
      start = std::chrono::system_clock::now();
      if ((m_inputFileSt != nullptr) && m_inputFileSt->isOpened())
      {
        *m_inputFileSt >> m_frameFileIn;
        cout<<"stream is opened"<<endl;
        if (m_patchLogic.connect.fileToTransIn)
        {
           cout<<"fileToTransIn"<<endl;
           dstTransIn = m_frameFileIn;
        }
        if (m_patchLogic.connect.fileToCh1)
        {
           dstCh1 = dstCh1 + m_frameFileIn;
        }
        if (m_patchLogic.connect.fileToCh2)
        {
           dstCh2 = dstCh2 + m_frameFileIn;
        }
        if (m_patchLogic.connect.fileToCh3)
        {
           dstCh3 = dstCh3 + m_frameFileIn;
        }
        if (m_patchLogic.connect.fileToCh4)
        {
           dstCh4 = dstCh4 + m_frameFileIn;
        }
        if (m_patchLogic.connect.fileToScreen)
        {
           m_display(m_frameFileIn);
        }
      }
      end = std::chrono::system_clock::now();
      elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
      cout<<"ms per frame file: "+to_string(elapsed.count())<<endl;
      if (dstTransIn.empty()) {break;}
      if (!m_inputBuff->pushMultiChanFrame(dstTransIn))
      {
        allowNewFrame = false;
      }
      else allowNewFrame = true;
    }
    else
    {
      if (!m_inputBuff->pushMultiChanFrame(dstTransIn))
      {
        allowNewFrame = false;
      }
      else allowNewFrame = true;
    }

    //TransOut mux
    if (m_outputBuff->pullMultiChanFrame(m_frameOut))
    {
        if (m_patchLogic.connect.transOutToFile)
        {
          cout<<"!!missing frame output"<<endl;
          *m_outputSt<<m_frameOut;
        }
        if (m_patchLogic.connect.transOutToCh1)
        {
          m_ch1<<m_frameOut;
        }
        if (m_patchLogic.connect.transOutToCh2)
        {
          m_ch2<<m_frameOut;
        }
        if (m_patchLogic.connect.transOutToCh3)
        {
          m_ch3<<m_frameOut;
        }
        if (m_patchLogic.connect.transOutToCh4)
        {
          m_ch4<<m_frameOut;
        }
        if (m_patchLogic.connect.transOutToScreen)
        {
          m_display(m_frameOut);
        }
    }

    //file mux


  }
  m_process.processStop();
  m_procVideo = false;
  LOG("m_procVideo  "+to_string(m_procVideo));
  LOG("finito");
}

void Synthesizer::registerDisplay(std::function<void(cv::Mat&)> callback)
{
    m_display = callback;
}

bool Synthesizer::isCamDevOpened()
{
    if (m_inputFileSt->isOpened() && (m_inputCamSt != nullptr)) return true;
    return false;
}

