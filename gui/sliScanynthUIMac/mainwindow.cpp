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

using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent, Synthesizer* synth) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_synth{synth},
    m_maskImage{},
    m_vidCapture{},
    m_vidWriter{},
    m_capWidth{},
    m_capHeight{}
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
    m_vidCapture = VideoCapture(filename.toUtf8().constData());
    m_capWidth = static_cast<int>(m_vidCapture.get(CAP_PROP_FRAME_WIDTH));
    m_capHeight = static_cast<int>(m_vidCapture.get(CAP_PROP_FRAME_HEIGHT));
    m_synth->setInputStream(make_unique<VideoCapture>(m_vidCapture));
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
    m_vidWriter.open(filename.toUtf8().constData(), VideoWriter::fourcc('F', 'M', 'P', '4'),
                     static_cast<int>(m_vidCapture.get(CAP_PROP_FPS))/2, Size(m_capWidth, m_capHeight), true);
    m_synth->setOutputStream(make_unique<VideoWriter>(m_vidWriter));
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
