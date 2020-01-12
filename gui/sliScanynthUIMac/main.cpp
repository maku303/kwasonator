#include "mainwindow.h"
#include <QApplication>
#include <qtimer.h>
#include <qobject.h>
#include <memory.h>
#include "../../src/synthesizer.h"
#include <opencv4/opencv2/core/cvstd.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include "logger.h"

using namespace std;
using namespace cv;



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Display disp;
    Logger::init();
    Synthesizer synth{};
    synth.registerDisplay(&Display::ImShow);
    MainWindow w{nullptr, &synth};
    w.show();


    return a.exec();
}
