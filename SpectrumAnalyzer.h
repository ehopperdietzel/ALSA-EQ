#ifndef SPECTRUMANALYZER_H
#define SPECTRUMANALYZER_H

#include <QWidget>
#include <QPen>
#include "AudioProcess.h"


class SpectrumAnalyzer : public QWidget
{
    Q_OBJECT
public:
    explicit SpectrumAnalyzer(QWidget *parent = nullptr);
    uint periodSize = BUFFER_SIZE;
    float buffer[BUFFER_SIZE];
    FFTData fft[BUFFER_SIZE];

    float pi = M_PI;
    float pi2 = pi*2.f;
    uint sampleHalf = 22050;


private:
    QPen barPen;

    void calculateFFT();
    void paintEvent(QPaintEvent *event) override;

public slots:
    void getData(float *buffer);

};

#endif // SPECTRUMANALYZER_H
