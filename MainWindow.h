#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSlider>
#include <QLabel>
#include <QThread>

#include "AudioProcess.h"
#include "SpectrumAnalyzer.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

    QThread audioThread;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    // Audio
    bool playing = false;
    AudioProcess *audioProcess = new AudioProcess;

    // UI Stuff
    QWidget *freqSliderContainer = new QWidget();
    QWidget *volumeSliderContainer = new QWidget();
    QWidget *centralWidget = new QWidget();

    QHBoxLayout *freqSliderLayout = new QHBoxLayout(freqSliderContainer);
    QHBoxLayout *volumeSliderLayout = new QHBoxLayout(volumeSliderContainer);
    QVBoxLayout *mainVerticalLayout = new QVBoxLayout(centralWidget);

    QSlider *freqSlider = new QSlider(Qt::Horizontal);
    QSlider *volumeSlider = new QSlider(Qt::Horizontal);

    QLabel *freqTitle = new QLabel("Frequency");
    QLabel *volumeTitle = new QLabel("Volume");
    QLabel *freqLabel = new QLabel();
    QLabel *volumeLabel = new QLabel();

    QPushButton *playbackBtn = new QPushButton("Play");

    // Spectrum analyzer
    SpectrumAnalyzer *spectrumAnalyzer = new SpectrumAnalyzer();


private slots:
    void volumeChanged(int vol);
    void frequencyChanged(int freq);
    void playbackBtnClick();
signals:
    void sendVolume(float volume);
    void sendFrequency(uint freq);
    void sendPlay(bool mode);



};
#endif // MAINWINDOW_H
