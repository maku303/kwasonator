#include <vector>
#include "buffer.h"
#include "logger.h"

using namespace std;

Buffer::Buffer(size_t buffSize, size_t chanSize):
    m_buffSize{buffSize},
    m_chanSize{chanSize},
    m_buff1half{vector<vector<cv::Mat>>(chanSize, vector<cv::Mat>(buffSize))},
    m_buff2half{vector<vector<cv::Mat>>(chanSize, vector<cv::Mat>(buffSize))},
    m_buff1State{vector<halfState>(chanSize, halfState::notReady)},
    m_buff2State{vector<halfState>(chanSize, halfState::notReady)},
    m_buff1ReadPtrIdx{vector<size_t>(chanSize, 0)},
    m_buff2ReadPtrIdx{vector<size_t>(chanSize, 0)},
    m_buff1WritePtrIdx{vector<size_t>(chanSize, 0)},
    m_buff2WritePtrIdx{vector<size_t>(chanSize, 0)},
    m_activeHalf{false}
{
}

Buffer::~Buffer()
{
}

bool Buffer::pushFrame(cv::Mat& frame, size_t chanInd, bool firstHalf)
{
  if (firstHalf)
  {
    m_buff1half[chanInd][m_buff1WritePtrIdx[chanInd]] = move(frame);
    LOG(" m_buff1half ");
    m_buff1WritePtrIdx[chanInd] = (m_buff1WritePtrIdx[chanInd] + 1) % m_buffSize;
    if (m_buff1WritePtrIdx[chanInd] == 0)
    {
      m_buff1State[chanInd] = halfState::ready;
      LOG("  m_buff1State = halfState::ready");
    }
    return true;
  }
  else
  {
    LOG("element size: "+to_string(frame.cols));
    m_buff2half[chanInd][m_buff2WritePtrIdx[chanInd]] = move(frame);
    LOG(" m_buff2half ");
    m_buff2WritePtrIdx[chanInd] = (m_buff2WritePtrIdx[chanInd] + 1) % m_buffSize;
    if (m_buff2WritePtrIdx[chanInd] == 0)
    {
      m_buff2State[chanInd] = halfState::ready;
      LOG("  m_buff2State = halfState::ready");
    }
    return true;
  }
}

bool Buffer::pullFrame(cv::Mat& frame, size_t chanInd, bool firstHalf)
{
  if (firstHalf)
  {
    frame = move(m_buff1half[chanInd][m_buff1ReadPtrIdx[chanInd]]);
    LOG(" m_buff1half ");
    m_buff1ReadPtrIdx[chanInd] = (m_buff1ReadPtrIdx[chanInd] + 1) % m_buffSize;
    if (m_buff1ReadPtrIdx[chanInd] == 0)
    {
      m_buff1State[chanInd] = halfState::notReady;
      LOG("  m_buff1State = halfState::notReady");
    }
    return true;
  }
  else
  {
    frame = move(m_buff2half[chanInd][m_buff2ReadPtrIdx[chanInd]]);
    LOG(" m_buff1half ");
    m_buff2ReadPtrIdx[chanInd] = (m_buff2ReadPtrIdx[chanInd] + 1) % m_buffSize;
    if (m_buff2ReadPtrIdx[chanInd] == 0)
    {
      m_buff2State[chanInd] = halfState::notReady;
      LOG("  m_buff2State = halfState::notReady");
    }
    return true;
  }
}

bool Buffer::pushMultiChanFrame(cv::Mat& frame)
{
  vector<cv::Mat> tempVec{3};
  split(frame, tempVec);
  assert(tempVec.size() == m_chanSize);
  if (isHalfNotReady(m_buff1State) && (!isFirstActiveHalf()))
  {
    for (size_t i = 0; i < m_chanSize; ++i)
    {
      pushFrame(tempVec[i], i, true);
    }
    return true;
  }
  else if (isHalfNotReady(m_buff2State) && isFirstActiveHalf())
  {
    for (size_t i = 0; i < m_chanSize; ++i)
    {
      pushFrame(tempVec[i], i, false);
    }
    return true;
  }
  return false;
}

bool Buffer::pushMultiChanFrame(vector<cv::Mat>& frame)
{
  assert(frame.size() == m_chanSize);
  if (isHalfNotReady(m_buff1State) && (!isFirstActiveHalf()))
  {
    for (size_t i = 0; i < m_chanSize; ++i)
    {
      pushFrame(frame[i], i, true);
    }
    return true;
  }
  else if (isHalfNotReady(m_buff2State) && isFirstActiveHalf())
  {
    for (size_t i = 0; i < m_chanSize; ++i)
    {
      pushFrame(frame[i], i, false);
    }
    return true;
  }
  return false;
}

bool Buffer::pullMultiChanFrame(cv::Mat& frame)
{
  vector<cv::Mat> tempVec{3};
  if (isHalfReady(m_buff1State) && (!isFirstActiveHalf()))
  {
    switchHalf();
    for (size_t i = 0; i < m_chanSize; ++i)
    {
      pullFrame(tempVec[i], i, true);
    }
    merge(tempVec, frame);
    return true;
  }
  else if (isHalfReady(m_buff2State) && (isFirstActiveHalf()))
  {
    switchHalf();
    for (size_t i = 0; i < m_chanSize; ++i)
    {
      pullFrame(tempVec[i], i, false);
    }
    merge(tempVec, frame);
    return true;
  }
  return false;
}

bool Buffer::pullMultiChanFrame(vector<cv::Mat>& frame)
{
  if (isHalfReady(m_buff1State) && (!isFirstActiveHalf()))
  {
    switchHalf();
    for (size_t i = 0; i < m_chanSize; ++i)
    {
      pullFrame(frame[i], i, true);
    }
    return true;
  }
  else if (isHalfReady(m_buff2State) && (isFirstActiveHalf()))
  {
    switchHalf();
    for (size_t i = 0; i < m_chanSize; ++i)
    {
      pullFrame(frame[i], i, false);
    }
    return true;
  }
  return false;
}

bool Buffer::isHalfNotReady(std::vector<halfState>& buffState)
{
  for (size_t i = 0; i < m_chanSize; ++i)
  {
    if (buffState[i] == halfState::ready) return false;
  }
  return true;
}

bool Buffer::isHalfReady(std::vector<halfState>& buffState)
{
  for (size_t i = 0; i < m_chanSize; ++i)
  {
    if (buffState[i] == halfState::notReady) return false;
  }
  return true;
}

bool Buffer::isFirstActiveHalf()
{
  if (m_activeHalf) return false;
  return true;
}

void Buffer::switchHalf()
{
  m_activeHalf = ~m_activeHalf;
}
