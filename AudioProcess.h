#ifndef AUDIOPROCESS_H
#define AUDIOPROCESS_H

#include <QObject>
#include <alsa/asoundlib.h>

struct FFTData
{
    float R = 0;
    float I = 0;
    float amplitude = 0;
    float phase = 0;
};

// The size of the internal ALSA buffer ( The period buffer is the half )
#define BUFFER_SIZE 512

class AudioProcess : public QObject
{
    Q_OBJECT

public:
    AudioProcess();
private:
    float volume = 0.f;
    // Audio
    bool playing = false;
    uint sampleRate = 44100;
    snd_pcm_uframes_t bufferSize = 1024;
    snd_pcm_uframes_t periodSize = 512;

    float frequency = 0;
    float time = 0.f;
    float pi = M_PI;
    float pi2 = pi*2.f;
    float theta = 0;
    float inc = 0;

    float writesCount = 0;


    float buffer[BUFFER_SIZE];


    // ALSA
    int err;
    snd_pcm_t *playback_handle;
    snd_pcm_hw_params_t *hw_params;
    snd_pcm_sframes_t framesWritten = 1;
    snd_async_handler_t *pcm_callback;

    void loop();


public slots:
    void play(bool mode);
    void volumeChanged(float vol);
    void frequencyChanged(uint freq);


signals:
    void sendBuffer(float *buffer);

};

#endif // AUDIOPROCESS_H
