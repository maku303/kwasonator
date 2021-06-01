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

    void on_checkBox_CamToFile_toggled(bool checked);

    void on_checkBox_CamToTransIn_toggled(bool checked);

    void on_checkBox_CamToCh1_toggled(bool checked);

    void on_checkBox_FileToTransIn_toggled(bool checked);

    void on_checkBox_FileToCh1_toggled(bool checked);

    void on_checkBox_FileToCh2_toggled(bool checked);

    void on_checkBox_FileToCh3_toggled(bool checked);

    void on_checkBox_TransOutToFile_toggled(bool checked);

    void on_checkBox_FileToCh4_toggled(bool checked);

    void on_checkBox_TransOutToCh1_toggled(bool checked);

    void on_checkBox_TransOutToCh2_toggled(bool checked);

    void on_checkBox_TransOutToCh3_toggled(bool checked);

    void on_checkBox_TransOutToCh4_toggled(bool checked);

    void on_checkBox_PatternToFile_toggled(bool checked);

    void on_checkBox_PatternToTransIn_toggled(bool checked);

    void on_checkBox_PatternToCh1_toggled(bool checked);

    void on_checkBox_PatternToCh2_toggled(bool checked);

    void on_checkBox_PatternToCh3_toggled(bool checked);

    void on_checkBox_PatternToCh4_toggled(bool checked);

    void on_checkBox_CamToScreen_toggled(bool checked);

    void on_checkBox_FileToScreen_toggled(bool checked);

    void on_checkBox_TransOutToScreen_toggled(bool checked);

    void on_listWidget_currentRowChanged(int currentRow);

    bool initCam();

private:
    Ui::MainWindow *ui;
    Synthesizer* m_synth;
    cv::Mat m_maskImage;
    patchbayLogic& m_patchbayLog;
    int m_capWidth;
    int m_capHeight;
    int m_capFps;
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
