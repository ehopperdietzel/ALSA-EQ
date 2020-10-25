#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    //device_list(SND_PCM_STREAM_PLAYBACK);

    // Configure audio Stuff
    audioProcess->moveToThread(&audioThread);
    connect(this,&MainWindow::sendPlay,audioProcess,&AudioProcess::play);
    connect(this,&MainWindow::sendFrequency,audioProcess,&AudioProcess::frequencyChanged);
    connect(this,&MainWindow::sendVolume,audioProcess,&AudioProcess::volumeChanged);
    connect(audioProcess,&AudioProcess::sendBuffer,spectrumAnalyzer,&SpectrumAnalyzer::getData);
    connect(&audioThread,&QThread::finished,audioProcess,&AudioProcess::deleteLater);
    audioThread.start();
    audioThread.setPriority(QThread::Priority::HighestPriority);

    // Configure UI Stuff
    setCentralWidget(centralWidget);

    mainVerticalLayout->addWidget(freqTitle);
    mainVerticalLayout->addWidget(freqSliderContainer);
    mainVerticalLayout->addWidget(volumeTitle);
    mainVerticalLayout->addWidget(volumeSliderContainer);
    mainVerticalLayout->addWidget(playbackBtn);

    freqSliderLayout->addWidget(freqSlider);
    freqSliderLayout->addWidget(freqLabel);

    volumeSliderLayout->addWidget(volumeSlider);
    volumeSliderLayout->addWidget(volumeLabel);

    freqLabel->setFixedWidth(50);
    volumeLabel->setFixedWidth(50);

    freqSlider->setTickInterval(1);
    freqSlider->setFixedWidth(400);
    volumeSlider->setFixedWidth(400);

    freqSlider->setRange(0,22050);
    volumeSlider->setRange(0,10000);

    // Listen to sliders changes
    connect(freqSlider,&QSlider::valueChanged,this,&MainWindow::frequencyChanged);
    connect(volumeSlider,&QSlider::valueChanged,this,&MainWindow::volumeChanged);

    // Listen to the playback button click
    connect(playbackBtn,&QPushButton::clicked,this,&MainWindow::playbackBtnClick);

    // Set default values
    freqSlider->setValue(440);
    volumeSlider->setValue(6000);

}

void MainWindow::volumeChanged(int vol)
{
    float volume = (float)vol/10000.f;
    volumeLabel->setText(QString::number((int)(volume*100)) + " %");

    sendVolume(volume);
}

void MainWindow::frequencyChanged(int freq)
{
    freqLabel->setText(QString::number(freq) + " hz");
    sendFrequency((uint)freq);
}

void MainWindow::playbackBtnClick()
{
    playing = !playing;

    if(playing)
    {
        sendPlay(true);
    }
    else
    {
        sendPlay(false);
    }
}



MainWindow::~MainWindow(){}


