#ifndef SLITSCAN_H
#define SLITSCAN_H

#include <thread>
#include "processMod.h"
#include "utilities.h"
#include "mathDefs.h"

struct Params
{
  Params():
      delayBlue{},
      delayGreen{},
      delayRed{},
      currentFactor{},
      forgottenFactor{},
      blckoutBlue{},
      blckoutGreen{},
      blckoutRed{},
      updated{false}
  {
  }
  uint8_t delayBlue;
  uint8_t delayGreen;
  uint8_t delayRed;
  uint8_t currentFactor;
  uint8_t forgottenFactor;
  uint8_t blckoutBlue;
  uint8_t blckoutGreen;
  uint8_t blckoutRed;
  bool updated;
};

class SlitScan : public ProcessMod
{
public:
  SlitScan();
  ~SlitScan() override;
  void initParams(size_t height, size_t width, size_t* filterSize);
  void processStart() override;
  void processStop() override;
  void process(size_t ind) override;
  //cv::mat version
  void transform(cv::Mat& frame, std::vector<std::vector<cv::Mat>>& processBuffMat, std::vector<cv::Mat>& filterSet, cv::Mat& accu);
  void updateMaskSrc(cv::Mat& mask);
  void recalcMask();
  void recalcConvFilt();
  void frameCollector();
  bool allDataCollected();
  void registerParams(std::shared_ptr<Params>& params);
private:
  size_t m_height;
  size_t m_width;
  size_t m_filterSize[3];
  std::vector<std::shared_ptr<std::thread>> m_proc;
  std::shared_ptr<std::thread> m_collectProc;
  std::atomic<uint32_t> m_dataProcessed;
  bool m_procBusy;
  //opencv version (color x delayline x delayline x partFilter)
  std::vector<std::vector<std::vector<cv::Mat>>> m_processBuff;
  //opecv version color x delayline x partFilter
  std::vector<std::vector<cv::Mat>> m_transFilters;
  std::vector<std::vector<cv::Mat>> m_delayFiltMask;
  //1d conv filter in time domain
  std::vector<std::vector<uint8_t>> m_convFilt;
  std::vector<cv::Mat> m_tempAccu;
  std::vector<cv::Mat> m_tempPostFilt;
  std::vector<cv::Mat> m_currentInFrame;
  std::vector<cv::Mat> m_currentOutFrame;
  std::vector<uint8_t> m_attGain;
  std::shared_ptr<Params> m_rtParams;
  cv::Mat m_mask;
  static size_t threadCntr;

};

#endif // SLITSCAN_H
