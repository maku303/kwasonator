#include <string>
#include "logger.h"
#include "utilities.h"

using namespace std;
using namespace cv;

void slitScanTransform(vector<Mat*> inputBuff, Mat& output, int delayH, int delayV) {

	int rowsNum = (*inputBuff[0]).rows;
	int colsNum = (*inputBuff[0]).cols;
	delayV = delayV - 4;
  uint32_t rowToDelayVratio = rowsNum / delayV;
	for (size_t i = 0; i < rowsNum; ++i) {
		uint32_t frameInd = static_cast<uint32_t>(i) / rowToDelayVratio;
		float fadeFact = static_cast<float>(static_cast<uint32_t>(i) % (rowToDelayVratio)) / rowToDelayVratio;
		cv::Mat interpMat = (1 - fadeFact) * inputBuff[frameInd]->row(i) + (1*fadeFact) * inputBuff[frameInd + 1]->row(i);
		interpMat.copyTo(output.row(i));
	}
}

int slitScanTransformElem(vector<Mat*> inputBuff, Mat& output, Mat& delayMask, uint8_t delayGain)
{
  //TODO all constants out, create class!!!
  int rowsNum = inputBuff[0]->rows;
  int colsNum = inputBuff[0]->cols;
  float frames = static_cast<float>(inputBuff.size())  * delayGain / 30000;
  for (size_t i = 0; i < rowsNum; ++i)
  {
    for (size_t j = 0; j < colsNum; ++j)
    {
      output.at<uchar>(i, j) = inputBuff[static_cast<int>(delayMask.at<uchar>(i, j) * frames)]->at<uchar>(i, j);
    }
  }
  return 1;
}

int slitScanTransformElem2(Mat& inputBuff, Mat& output, Mat& delayMask, uint8_t delayGain)
{
  //TODO all constants out, this function is bottleneck, only for loops shall be left
  //TODO lack of fadings
  int rowsNum = inputBuff.size[0];
  int colsNum = inputBuff.size[1];
  int dimsNum = inputBuff.size[2];
  float frames = static_cast<float>(dimsNum)  * delayGain / 30000;
  Mat timeIind = delayMask * frames;
  for (size_t i = 0; i < rowsNum; ++i)
  {
    for (size_t j = 0; j < colsNum; ++j)
    {
      output.at<uchar>(i, j) = inputBuff.at<uchar>(i, j, timeIind.at<uchar>(i, j));
    }
  }
  return 1;
}

void transformVertical()
{

}

void transformHorizontal()
{

}


videoBuffer::videoBuffer(int size, Size vidSize):
    m_buffSize(size),
    m_vidSize(vidSize),
    m_buff(std::vector<cv::Mat>(m_buffSize)),
    m_count{0},
    m_pos{0}
{
	for (int i = 0; i < m_buffSize; ++i ) {
		m_buff[i] = Mat::zeros(m_vidSize.height, m_vidSize.width, CV_8U);
	}
}

videoBuffer::~videoBuffer()
{
  cout<<"videoBuffer::~videoBuffer()"<<endl;
}

void videoBuffer::addNewElement(cv::Mat& el)
{
  m_pos = m_count % m_buffSize;
  m_buff[m_pos] = el.clone();
  m_count++;
}

std::vector<cv::Mat*> videoBuffer::getBuffer()
{
	std::vector<cv::Mat*> result(m_buffSize);
	for (size_t i = 0; i < m_buffSize; ++i) {
		int modulo = (m_pos - i - 1) % m_buffSize;
		if (modulo < 0) {
			modulo = m_buffSize + modulo;
		}
		//std::cout<<"index num: "<<modulo<<"\n";
		result[i] = &m_buff[modulo];
	}
	return result;
}

//move to GPU
void convolve(std::vector<vector<cv::Mat>>& frame, std::vector<vector<cv::Mat>>& filter)
{
  size_t frameDim3Size = frame[0].size();
  size_t frameDim1Size = static_cast<size_t>(frame[0][0].rows);
  size_t frameDim2Size = static_cast<size_t>(frame[0][0].cols);
  size_t filtDim3Size = filter[0].size();
  size_t filtDim1Size = static_cast<size_t>(filter[0][0].rows) >> 1;
  size_t filtDim2Size = static_cast<size_t>(filter[0][0].cols) >> 1;

  for (size_t i = filtDim1Size; i < frameDim1Size - filtDim1Size - 1; ++i)
  {
    for (size_t j = filtDim2Size; j < frameDim2Size - filtDim2Size - 1; ++j)
    {
      for (size_t k = 0; k < frameDim3Size; ++k)
      {
        cv::Mat framePartc1 = frame[0][k](cv::Rect(j - filtDim2Size, i - filtDim1Size, 3, 3));
        cv::Mat framePartc2 = frame[1][k](cv::Rect(i - filtDim1Size, j - filtDim2Size, 3, 3));
        cv::Mat framePartc3 = frame[2][k](cv::Rect(i - filtDim1Size, j - filtDim2Size, 3, 3));
        //WIP, TB implemented ...
      }
    }
  }


}

