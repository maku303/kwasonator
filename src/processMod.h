#ifndef PROCESSMOD_H
#define PROCESSMOD_H

#include <memory>
#include <opencv4/opencv2/core/core.hpp>
#include "buffer.h"

class ProcessMod
{
  enum class bufferStat {notReady, ready, busy, processed};
public:
  ProcessMod(size_t reqBuffSize);
  virtual ~ProcessMod() = 0;
  virtual void processStart() = 0;
  virtual void processStop() = 0;
  virtual void process(size_t ind) = 0;
  virtual void bindInBuffers(std::shared_ptr<Buffer>& buff);
  virtual void bindOutBuffers(std::shared_ptr<Buffer>& buff);
protected:
  std::shared_ptr<Buffer> m_inputBuff;
  std::shared_ptr<Buffer> m_outputBuff;
  size_t m_reqBuffSize;
};

#endif // PROCESSMOD_H
