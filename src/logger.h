#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>

#define LOG(x) Logger::Log(std::string(__FUNCTION__) + " " + x)

class Logger
{
public:
    static std::string fileName;
    static std::ofstream out_stream;
    static void Log(std::string info);
    static void init();
    static void onExit();
private:
    Logger()
    {
    }
};

#endif // LOGGER_H
