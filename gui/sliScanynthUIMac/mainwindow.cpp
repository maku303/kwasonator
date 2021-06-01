#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <memory>
#include <opencv4/opencv2/core/cvstd.hpp>
//#include <opencv2/core/core.hpp>
#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/video.hpp>
#include<opencv4/opencv2/objdetect/objdetect.hpp>
#include<opencv4/opencv2/imgproc/imgproc.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include "../../src/synthesizer.h"
#include <chrono>
#include <thread>
#include <string>
#include "logger.h"

using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent, Synthesizer* synth) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_synth{synth},
    m_maskImage{},
    m_patchbayLog{synth->registerPatchbayLogic()},
    m_capWidth{1920},
    m_capHeight{1080},
    m_capFps{25}
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_verticalSlider_valueChanged(int value)
{
    m_synth->setDelayRed(static_cast<uint8_t>(value));
}

void MainWindow::on_pushButtonLoadMovie_clicked()
{
    QString filename = ui->plainTextEditLoadMovie->toPlainText();
    unique_ptr<VideoCapture> vidCapture = make_unique<VideoCapture>(VideoCapture(filename.toUtf8().constData()));
    //m_capWidth = static_cast<int>(vidCapture->get(CAP_PROP_FRAME_WIDTH));
    //m_capHeight = static_cast<int>(vidCapture->get(CAP_PROP_FRAME_HEIGHT));
    //m_capFps = static_cast<int>(vidCapture->get(CAP_PROP_FPS) / 2);
    m_synth->setInputFileStream(std::move(vidCapture));
}

void MainWindow::on_verticalSlider_2_valueChanged(int value)
{
    m_synth->setDelayGreen(static_cast<uint8_t>(value));
}

void MainWindow::on_verticalSlider_3_valueChanged(int value)
{
    m_synth->setDelayBlue(static_cast<uint8_t>(value));
}

void MainWindow::on_pushButtonLoadMask_clicked()
{
    QString filename = ui->plainTextEditLoadDelayMask->toPlainText();
    m_maskImage = imread(filename.toUtf8().constData(), IMREAD_COLOR );
    if (m_maskImage.size().height)
    {
        cout<<"Mask Loaded "<<endl;
    }
    m_synth->setDelayMask(m_maskImage);
}


void MainWindow::on_pushButtonLoadOutput_clicked()
{
    QString filename = ui->plainTextEditLoadOutput->toPlainText();
    unique_ptr<VideoWriter> vidWriter{make_unique<VideoWriter>()};
    vidWriter->open(filename.toUtf8().constData(), VideoWriter::fourcc('F', 'M', 'P', '4'),
                     m_capFps, Size(m_capWidth, m_capHeight), true);
    m_synth->setOutputStream(std::move(vidWriter));
}

void MainWindow::on_pushButton_clicked()
{

    if (m_synth->isRunning())
    {
        cout<<"stop requested"<<endl;
        m_synth->stopSynth();
    }
    else
    {
        cout<<"start requested"<<endl;
        m_synth->startSynth();
    }
}

void MainWindow::on_verticalSlider_redGain_valueChanged(int value)
{
    m_synth->setRedGain(static_cast<uint8_t>(value));
}

void MainWindow::on_verticalSlider_greenGain_valueChanged(int value)
{
    m_synth->setGreenGain(static_cast<uint8_t>(value));
}

void MainWindow::on_verticalSlider_blueGain_valueChanged(int value)
{
    m_synth->setBlueGain(static_cast<uint8_t>(value));
}

void MainWindow::on_verticalSlider_currentFactor_valueChanged(int value)
{
    m_synth->setCurrentFactor(static_cast<uint8_t>(value));
    cout<<"currentFactor "<<value<<endl;
}

void MainWindow::on_verticalSlider_forgottenFactor_valueChanged(int value)
{
    m_synth->setForgottenFactor(static_cast<uint8_t>(value));
    cout<<"forgottenFactor "<<value<<endl;
}


void MainWindow::on_checkBox_CamToFile_toggled(bool checked)
{
  LOG("");
  if (initCam()) m_patchbayLog.connect.camToFile = checked;
}

void MainWindow::on_checkBox_CamToTransIn_toggled(bool checked)
{
  LOG("");
  if (initCam()) m_patchbayLog.connect.camToTransIn = checked;
}

void MainWindow::on_checkBox_CamToCh1_toggled(bool checked)
{
   LOG("");
   if (initCam()) m_patchbayLog.connect.camToCh1 = checked;
}

void MainWindow::on_checkBox_FileToTransIn_toggled(bool checked)
{
   LOG("");
   m_patchbayLog.connect.fileToTransIn = checked;
}

void MainWindow::on_checkBox_FileToCh1_toggled(bool checked)
{
   LOG("");
   m_patchbayLog.connect.fileToCh1 = checked;
}

void MainWindow::on_checkBox_FileToCh2_toggled(bool checked)
{
   LOG("");
   m_patchbayLog.connect.fileToCh2 = checked;
}

void MainWindow::on_checkBox_FileToCh3_toggled(bool checked)
{
   LOG("");
   m_patchbayLog.connect.fileToCh3 = checked;
}

void MainWindow::on_checkBox_TransOutToFile_toggled(bool checked)
{
   LOG("");
   m_patchbayLog.connect.transOutToFile = checked;
}

void MainWindow::on_checkBox_FileToCh4_toggled(bool checked)
{
   LOG("");
   m_patchbayLog.connect.fileToCh4 = checked;
}

void MainWindow::on_checkBox_TransOutToCh1_toggled(bool checked)
{
   LOG("");
   m_patchbayLog.connect.transOutToCh1 = checked;
}

void MainWindow::on_checkBox_TransOutToCh2_toggled(bool checked)
{
   LOG("");
   m_patchbayLog.connect.transOutToCh2 = checked;
}

void MainWindow::on_checkBox_TransOutToCh3_toggled(bool checked)
{
   LOG("");
   m_patchbayLog.connect.transOutToCh3 = checked;
}

void MainWindow::on_checkBox_TransOutToCh4_toggled(bool checked)
{
   LOG("");
   m_patchbayLog.connect.transOutToCh4 = checked;
}

void MainWindow::on_checkBox_PatternToFile_toggled(bool checked)
{
   LOG("");
   m_patchbayLog.connect.patternToFile = checked;
}

void MainWindow::on_checkBox_PatternToTransIn_toggled(bool checked)
{
   LOG("");
   m_patchbayLog.connect.patternToTransIn = checked;
}

void MainWindow::on_checkBox_PatternToCh1_toggled(bool checked)
{
   LOG("");
   m_patchbayLog.connect.patternToCh1 = checked;
}

void MainWindow::on_checkBox_PatternToCh2_toggled(bool checked)
{
   LOG("");
   m_patchbayLog.connect.patternToCh2 = checked;
}

void MainWindow::on_checkBox_PatternToCh3_toggled(bool checked)
{
   LOG("");
   m_patchbayLog.connect.patternToCh3 = checked;
}

void MainWindow::on_checkBox_PatternToCh4_toggled(bool checked)
{
   LOG("");
   m_patchbayLog.connect.patternToCh4 = checked;
}

void MainWindow::on_checkBox_CamToScreen_toggled(bool checked)
{
    LOG("");
    if (initCam()) m_patchbayLog.connect.camToScreen = checked;
}

void MainWindow::on_checkBox_FileToScreen_toggled(bool checked)
{
    LOG("");
    m_patchbayLog.connect.fileToScreen = checked;
}


void MainWindow::on_checkBox_TransOutToScreen_toggled(bool checked)
{
    LOG("");
    m_patchbayLog.connect.transOutToScreen = checked;
}

void MainWindow::on_listWidget_currentRowChanged(int currentRow)
{
    //1080x1920
    if (currentRow == 0)
    {
      m_capWidth = 1920;
      m_capHeight = 1080;
      m_capFps = 25;
    }
    //768x1360
    else if (currentRow == 1)
    {
      m_capWidth = 1360;
      m_capHeight = 768;
      m_capFps = 25;
    }
}

bool MainWindow::initCam()
{
    if (!m_synth->isCamDevOpened())
    {
      unique_ptr<VideoCapture> vidCam{make_unique<VideoCapture>()};
      if (vidCam->open(cv::CAP_ANY))
      {
        m_synth->setInputCamStream(std::move(vidCam));
        return true;
      }
      else
        cout<<"camera device not opened"<<endl;
        return false;
    }
    return true;
}
