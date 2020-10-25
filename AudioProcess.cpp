#include "AudioProcess.h"
#include <QDebug>
#include <QApplication>
#include <QtMath>


AudioProcess::AudioProcess()
{
    // Open the system default audio output device
    if ((err = snd_pcm_open (&playback_handle, "plughw:0", SND_PCM_STREAM_PLAYBACK, 0)) < 0)
    {
        qDebug() << "Cannot open audio device" << snd_strerror (err);
        exit (1);
    }

    // Allocates the hardware parameters structure ( to describe/configure the output stream )
    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0)
    {
        qDebug() << "Cannot allocate hardware parameter structure" << snd_strerror (err);
        exit (1);
    }

    // Initializes the hardware parameters structure
    if ((err = snd_pcm_hw_params_any (playback_handle, hw_params)) < 0)
    {
        qDebug() << "Cannot initialize hardware parameter structure" << snd_strerror (err);
        exit (1);
    }

    // Set the sample format ( 32 bit float little endian )
    if ((err = snd_pcm_hw_params_set_format (playback_handle, hw_params, SND_PCM_FORMAT_FLOAT_LE)) < 0)
    {
        qDebug() << "Cannot set sample format" << snd_strerror (err);
        exit (1);
    }

    // Set the access mode to interleaved ( The signal is MONO so it doesn't really matter )
    if ((err = snd_pcm_hw_params_set_access (playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
    {
        qDebug() << "Cannot set access type" << snd_strerror (err);
        exit (1);
    }

    // Sets the sample rate to 44100
    if ((err = snd_pcm_hw_params_set_rate_near (playback_handle, hw_params, &sampleRate, 0)) < 0)
    {
        qDebug() << "Cannot set sample rate" << snd_strerror (err);
        exit (1);
    }

    // Sets the number of channels to 1 ( MONO )
    if ((err = snd_pcm_hw_params_set_channels (playback_handle, hw_params, 1)) < 0)
    {
        qDebug() << "Cannot set channel count" << snd_strerror (err);
        exit (1);
    }


    if ((err = snd_pcm_hw_params_set_buffer_size_near(playback_handle,hw_params, &bufferSize)) < 0 )
    {
        qDebug() << "Cannot set buffer size" << snd_strerror (err);
        exit (1);
    }

    if ((err = snd_pcm_hw_params_set_period_size_near(playback_handle,hw_params, &periodSize,NULL)) < 0 )
    {
        qDebug() << "Cannot set period size" << snd_strerror (err);
        exit (1);
    }


    // Applies the hardware structure to the device
    if ((err = snd_pcm_hw_params (playback_handle, hw_params)) < 0)
    {
        qDebug() << "Cannot set parameters" << snd_strerror (err);
        exit (1);
    }

    // Deletes the hw structure
    snd_pcm_hw_params_free (hw_params);


    // Prepare the device for playback
    if ((err = snd_pcm_prepare (playback_handle)) < 0)
    {
        fprintf (stderr, "Cannot prepare audio interface for use (%s)\n",
        snd_strerror (err));
        exit (1);
    }

}

void AudioProcess::loop()
{
    while(playing)
    {
        for(uint i = 0; i < BUFFER_SIZE; i++)
        {
            buffer[i] = qSin(theta)*volume;
            theta += inc;
            if(theta > 2.f*M_PI)
            {
                theta -= 2.f*M_PI;
            }
        }

        if(writesCount == (sampleRate/BUFFER_SIZE)/16)
        {
            emit sendBuffer(buffer);
            writesCount = 0;
        }
        else
        {
            writesCount++;
        }

        snd_pcm_writei(playback_handle, buffer,BUFFER_SIZE);

        QApplication::processEvents();

    }
}

void AudioProcess::play(bool mode)
{
    playing = mode;

    if(mode)
    {
        qDebug() << "Playing...";
        snd_pcm_prepare (playback_handle);
        loop();
    }
    else
    {
        qDebug() << "Stopped...";
    }
}

void AudioProcess::volumeChanged(float vol)
{
    volume = vol;
}

void AudioProcess::frequencyChanged(uint freq)
{
    frequency = float(freq);
    inc = (pi2*frequency)/float(sampleRate);
}


