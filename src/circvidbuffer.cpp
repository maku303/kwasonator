#include "circvidbuffer.h"

using namespace std;
using namespace cv;

CircVidBuffer::CircVidBuffer():
    m_buff{},
    m_ind{0}
{

}

CircVidBuffer& CircVidBuffer::operator >> (cv::Mat& image)
{
  if (m_buff.size() > 0)
  {
    m_ind = (m_ind + 1) % m_buff.size();
    image = m_buff[m_ind];
  }
  return *this;
}

CircVidBuffer& CircVidBuffer::operator << (cv::Mat& image)
{
  m_buff.push_back(image);
  return *this;
}

void clearBuff()
{

}
