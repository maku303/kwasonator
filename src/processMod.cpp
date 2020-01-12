#include "processMod.h"

using namespace std;

ProcessMod::ProcessMod(size_t reqBuffSize):
    m_inputBuff{},
    m_outputBuff{},
    m_reqBuffSize{reqBuffSize}
{
}

ProcessMod::~ProcessMod()
{
}

void ProcessMod::bindInBuffers(shared_ptr<Buffer>& buff)
{
  m_inputBuff = buff;
}

void ProcessMod::bindOutBuffers(shared_ptr<Buffer>& buff)
{
  m_outputBuff = buff;
}
