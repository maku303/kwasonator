#include "logger.h"

std::string Logger::fileName = "/Users/maku/Repositories/src-slitScanSynth/cpp/logs.txt";
std::ofstream Logger::out_stream;


void Logger::Log(std::string info)
{
  Logger::out_stream << info << std::endl;
}

void Logger::init()
{
  Logger::out_stream.open(Logger::fileName);
}

void Logger::onExit()
{
  Logger::out_stream.close();
}
