#include <memory>
#include "slitScan.h"
#include "logger.h"
#include "utilities.h"

using namespace std;

size_t SlitScan::threadCntr = 0;

SlitScan::SlitScan():
  ProcessMod{3},
  m_height{},
  m_width{},
  m_filterSize{},
  m_proc{},
  m_dataProcessed{0x7},
  m_procBusy{false},
  m_delayFiltMask{vector<cv::Mat>(3)},
  m_tempAccu{vector<cv::Mat>(3)},
  m_tempPostFilt{vector<cv::Mat>(3)},
  m_currentInFrame{vector<cv::Mat>(3)},
  m_currentOutFrame{vector<cv::Mat>(3)},
  m_attGain{vector<uint8_t>(3, 0)},
  m_rtParams{make_shared<Params>()},
  m_mask{}
{
   LOG("");
}

SlitScan::~SlitScan()
{
  LOG("");
}

void SlitScan::initParams(size_t height, size_t width, size_t* filterSize)
{
    m_height = height;
    m_width = width;
    m_filterSize[0] = filterSize[0];
    m_filterSize[1] = filterSize[1];
    m_filterSize[2] = filterSize[2];
    m_processBuff = vector<vector<vector<cv::Mat>>>(3, vector<vector<cv::Mat>>(filterSize[2], vector<cv::Mat>(filterSize[2])));
    cv::Mat initMat{static_cast<int>(m_height), static_cast<int>(m_width), CV_8U, cv::Scalar(0)};
    for (size_t i = 0; i < filterSize[2]; ++i)
    {
      for (size_t j = 0; j < filterSize[2]; ++j)
      {
        m_processBuff[0][i][j] = initMat.clone();
        m_processBuff[1][i][j] = initMat.clone();
        m_processBuff[2][i][j] = initMat.clone();
      }
    }
    m_tempAccu[0] = initMat.clone();
    m_tempAccu[1] = initMat.clone();
    m_tempAccu[2] = initMat.clone();
    m_transFilters = vector<vector<cv::Mat>>(3, vector<cv::Mat>(m_filterSize[2]));
    m_delayFiltMask = vector<vector<cv::Mat>>(3, vector<cv::Mat>(m_filterSize[2]));
    m_convFilt = vector<vector<uint8_t>>(3, vector<uint8_t>(m_filterSize[2], 0));
    cv::Mat convInitMat{static_cast<int>(filterSize[0]), static_cast<int>(filterSize[1]), CV_8U, cv::Scalar(0)};
    for (size_t i = 0; i < m_filterSize[2]; ++i)
    {
      m_transFilters[0][i] = initMat.clone();
      m_transFilters[1][i] = initMat.clone();
      m_transFilters[2][i] = initMat.clone();
      m_delayFiltMask[0][i] = initMat.clone();
      m_delayFiltMask[1][i] = initMat.clone();
      m_delayFiltMask[2][i] = initMat.clone();
    }
    //recalcMask();
}

void SlitScan::processStart()
{
  m_procBusy = true;
  m_proc.push_back(make_shared<thread>(thread{&SlitScan::process, this, 0}));
  m_proc.push_back(make_shared<thread>(thread{&SlitScan::process, this, 1}));
  m_proc.push_back(make_shared<thread>(thread{&SlitScan::process, this, 2}));
  m_collectProc = make_shared<thread>(thread{&SlitScan::frameCollector, this});
}

void SlitScan::processStop()
{
  m_procBusy = false;
  for (size_t i = 0; i < m_proc.size(); ++i)
  {
    m_proc[i]->join();
  }
  m_proc.clear();
  m_collectProc->join();
  m_dataProcessed = 0x7;
  m_currentInFrame = vector<cv::Mat>(3);
  m_currentOutFrame = vector<cv::Mat>(3);
}

void SlitScan::process(size_t ind)
{
  LOG("thread starter: "+ to_string(ind) + " , thread counter: " + to_string(threadCntr));

  while (m_procBusy)
  {
    if (m_dataProcessed & (1 << ind))
    {
      continue;
    }

    //transformation part
    transform(m_currentInFrame[ind], m_processBuff[ind], m_transFilters[ind], m_tempAccu[ind]);
    m_currentOutFrame[ind] = move(m_currentInFrame[ind]);
    m_dataProcessed |= (1 << ind);
  }
}

void SlitScan::transform(cv::Mat& frame, vector<vector<cv::Mat>>& processBuffMat, vector<cv::Mat>& filterSet, cv::Mat& accu)
{
  processBuffMat.insert(processBuffMat.begin(), processBuffMat.end() - 1, processBuffMat.end());
  for (size_t i = 0; i < m_filterSize[2]; ++i)
  {
    processBuffMat[i].insert(processBuffMat[i].end(), processBuffMat[i].begin(), processBuffMat[i].begin() + 1);
    processBuffMat[i].erase(processBuffMat[i].begin());
    processBuffMat[0][i] = move(processBuffMat[m_filterSize[2]][i]);
    processBuffMat[0][i] = frame.mul(filterSet[i]) / 255;
    accu += processBuffMat[i][0];
  }
  processBuffMat.pop_back();
  frame = accu.clone();
  accu = 0;
}

void SlitScan::updateMaskSrc(cv::Mat& mask)
{
    m_mask = move(mask);
}

void SlitScan::recalcMask()
{
  auto start = std::chrono::system_clock::now();
  vector<cv::Mat> rgb;
  split(m_mask, rgb);
  size_t delayInd0{0}, delayInd1{0}, delayInd2{0};
  for (size_t i = 0; i < m_height; ++i)
  {
    for (size_t j = 0; j < m_width; ++j)
    {
      delayInd0 = static_cast<uint8_t>((rgb[0].at<uint8_t>(static_cast<int>(i), static_cast<int>(j))
                                       ) >> (8 + m_rtParams->delayRed - 5));
      if (delayInd0 > (m_filterSize[2] - 1)) delayInd0 = m_filterSize[2] - 1;
      delayInd1 = static_cast<uint8_t>((rgb[1].at<uint8_t>(static_cast<int>(i), static_cast<int>(j))
                                       ) >> (8 + m_rtParams->delayGreen - 5));
      if (delayInd1 > (m_filterSize[2] - 1)) delayInd1 = m_filterSize[2] - 1;
      delayInd2 = static_cast<uint8_t>((rgb[2].at<uint8_t>(static_cast<int>(i), static_cast<int>(j))
                                       ) >> (8 + m_rtParams->delayBlue - 5));
      if (delayInd2 > (m_filterSize[2] - 1)) delayInd2 = m_filterSize[2] - 1;
      m_delayFiltMask[0][delayInd0].at<uint8_t>(static_cast<int>(i), static_cast<int>(j)) = 0xff >> m_rtParams->blckoutRed;
      m_delayFiltMask[1][delayInd1].at<uint8_t>(static_cast<int>(i), static_cast<int>(j)) = 0xff >> m_rtParams->blckoutGreen;
      m_delayFiltMask[2][delayInd2].at<uint8_t>(static_cast<int>(i), static_cast<int>(j)) = 0xff >> m_rtParams->blckoutBlue;
    }
  }
  recalcConvFilt();
  m_transFilters.swap(m_delayFiltMask);
  for (size_t i = 0; i < m_filterSize[2]; ++i)
  {
    m_delayFiltMask[0][i] = 0;
    m_delayFiltMask[1][i] = 0;
    m_delayFiltMask[2][i] = 0;
  }
  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  LOG("ms per recalcMask: "+to_string(elapsed.count()));
}

void SlitScan::recalcConvFilt()
{
  m_delayFiltMask[0][0] = (m_delayFiltMask[0][0] * m_rtParams->currentFactor) / 255;
  m_delayFiltMask[1][0] = (m_delayFiltMask[1][0] * m_rtParams->currentFactor) / 255;
  m_delayFiltMask[2][0] = (m_delayFiltMask[2][0] * m_rtParams->currentFactor) / 255;
  for (size_t i = 1; i < m_filterSize[2]; ++i)
  {
    m_delayFiltMask[0][i] = (m_rtParams->currentFactor * m_delayFiltMask[0][i] + m_rtParams->forgottenFactor * m_delayFiltMask[0][i - 1]) / 255;
    m_delayFiltMask[1][i] = (m_rtParams->currentFactor * m_delayFiltMask[1][i] + m_rtParams->forgottenFactor * m_delayFiltMask[1][i - 1]) / 255;
    m_delayFiltMask[2][i] = (m_rtParams->currentFactor * m_delayFiltMask[2][i] + m_rtParams->forgottenFactor * m_delayFiltMask[2][i - 1]) / 255;
  }
}

void SlitScan::frameCollector()
{
  while (!m_inputBuff->pullMultiChanFrame(m_currentInFrame) && m_procBusy)
  {
  }
  m_dataProcessed = 0;

  while (m_procBusy)
  {
    if (((0x7 & m_dataProcessed) == 0x7) && m_outputBuff->pushMultiChanFrame(m_currentOutFrame))
    {
      while (!m_inputBuff->pullMultiChanFrame(m_currentInFrame) && m_procBusy)
      {
      }
      m_dataProcessed = 0;
    }
    else if (m_rtParams->updated)
    {
      recalcMask();
      m_rtParams->updated = false;
    }
  }
}

bool SlitScan::allDataCollected()
{
  uint32_t result{0};
  for (size_t i = 0; i < m_proc.size(); ++i)
  {
    result |= 1 << i;
  }
  return (result == m_dataProcessed);
}

void SlitScan::registerParams(shared_ptr<Params>& params)
{
  params = m_rtParams;
}
