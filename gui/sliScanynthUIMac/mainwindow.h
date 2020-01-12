#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <qtimer.h>
#include <qobject.h>
#include <opencv4/opencv2/core/cvstd.hpp>
//#include <opencv2/core/core.hpp>
#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/video.hpp>
#include<opencv4/opencv2/objdetect/objdetect.hpp>
#include<opencv4/opencv2/imgproc/imgproc.hpp>
#include "synthesizer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr, Synthesizer* synth = nullptr);
    ~MainWindow();

private slots:
    void on_verticalSlider_valueChanged(int value);

    void on_pushButtonLoadMovie_clicked();

    void on_verticalSlider_2_valueChanged(int value);

    void on_verticalSlider_3_valueChanged(int value);

    void on_pushButtonLoadMask_clicked();

    void on_pushButtonLoadOutput_clicked();

    void on_pushButton_clicked();

    void on_verticalSlider_redGain_valueChanged(int value);

    void on_verticalSlider_greenGain_valueChanged(int value);

    void on_verticalSlider_blueGain_valueChanged(int value);

    void on_verticalSlider_currentFactor_valueChanged(int value);

    void on_verticalSlider_forgottenFactor_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    Synthesizer* m_synth;
    cv::Mat m_maskImage;
    cv::VideoCapture m_vidCapture;
    cv::VideoWriter m_vidWriter;
    int m_capWidth;
    int m_capHeight;
};

class Display
{
public:
    Display()
    {

    }
    static void ImShow(cv::Mat& frame)
    {
        QTimer* timer = new QTimer();
        timer->moveToThread(qApp->thread());
        timer->setSingleShot(true);
        QObject::connect(timer, &QTimer::timeout, [=]()
        {
            /* main thread
             * //callback();*/
            cv::namedWindow("slitScan",1);
            cv::imshow("slitScan", frame );
            timer->deleteLater();
        });
        QMetaObject::invokeMethod(timer, "start", Qt::QueuedConnection, Q_ARG(int, 0));
    }
};

#endif // MAINWINDOW_H
