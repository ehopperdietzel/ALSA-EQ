#include "SpectrumAnalyzer.h"
#include <QPainter>
#include <QDebug>
#include <QtMath>

SpectrumAnalyzer::SpectrumAnalyzer(QWidget *parent) : QWidget(parent)
{
    move(200,200);
    resize(256,100);
    barPen.setWidth(3);
    barPen.setColor(Qt::yellow);
    show();
}

void SpectrumAnalyzer::calculateFFT()
{
    // Tamaño de la ventana
    int bufferSize = 512;

    // Frecuencia de muestreo
    int fs = 44100;

    // Delta k
    float kDelta = (fs/2)/bufferSize; // 0 ... 22050

    // Loop k
    for(int i = 0; i < bufferSize; i++)
    {
        // Calcula la frecuencia k
        int k = (i+1)*kDelta;

        // Reinicia los coeficientes
        fft[i].R = 0;
        fft[i].I = 0;

        // Loop n
        for(int n = 0; n < bufferSize; n++)
        {
            float arg = (2.f*M_PI*k*n)/fs;

            // Parte real
            fft[i].R += buffer[n]*qCos(arg);

            // Parte imaginaria
            fft[i].I += buffer[n]*qSin(arg);
        }

        // Calcula la magnitud
        fft[i].amplitude = qSqrt(qPow(fft[i].R,2.f) + qPow(fft[i].I,2.f))/256.f;

        // Calcula el ángulo
        fft[i].phase = qAtan(fft[i].I/fft[i].R);
    }

}

void SpectrumAnalyzer::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor("#444"));
    painter.drawRect(0,0,width(),height());

    painter.setPen(barPen);
    int x;
    for(int i = 0; i < 512; i++)
    {
        x = (i*width())/512.f;
        painter.drawLine(x,height(),x,height() - fft[i].amplitude*height());
    }
    int freqIndex = (512.f*8000.f)/22050.f;
    if(fft[freqIndex].amplitude > 0.8)
        qDebug() << fft[freqIndex].amplitude;

}

void SpectrumAnalyzer::getData(float *data)
{
    memcpy(buffer,data,sizeof(buffer));
    calculateFFT();
    repaint();
}
