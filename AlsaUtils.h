#ifndef ALSAUTILS_H
#define ALSAUTILS_H

#include <QDebug>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <locale.h>
#include <alsa/asoundlib.h>


bool isLittleEndian()
{
    short int number = 0x1;
    char *numPtr = (char*)&number;
    return (numPtr[0] == 1);
}


// List avaliable devices
static void device_list(snd_pcm_stream_t stream)
{
    // Card handle
    snd_ctl_t *handle;
    int err, dev, idx;
    snd_ctl_card_info_t *info;
    snd_pcm_info_t *pcminfo;
    snd_ctl_card_info_alloca(&info);
    snd_pcm_info_alloca(&pcminfo);

    // First card index
    int card = -1;

    // Look for the first card avaliable
    if (snd_card_next(&card) < 0 || card < 0)
    {
        qDebug() << "No soundcards found.";
        return;
    }

    qDebug() << "---- List of" << snd_pcm_stream_name(stream) << "Hardware Devices ----";

    // Loop the cards
    while (card >= 0)
    {

        char name[32];

        // Creates the card name (hw:card)
        sprintf(name, "hw:%d", card);

        // Open a card ( gets the card handle )
        if ((err = snd_ctl_open(&handle, name, 0)) < 0)
        {
            // If couldn't be open
            qDebug() << "Card" << card << ":" << snd_strerror(err);
            goto next_card;
        }

        // Gets the card info
        if ((err = snd_ctl_card_info(handle, info)) < 0)
        {
            qDebug() << "Control hardware info" << card << ":" << snd_strerror(err);
            snd_ctl_close(handle);
            goto next_card;
        }

        dev = -1;
        while (1)
        {
            unsigned int count;

            if (snd_ctl_pcm_next_device(handle, &dev) < 0)
                qDebug() << "snd_ctl_pcm_next_device";

            if (dev < 0)
                break;

            snd_pcm_info_set_device(pcminfo, dev);
            snd_pcm_info_set_subdevice(pcminfo, 0);
            snd_pcm_info_set_stream(pcminfo, stream);

            if ((err = snd_ctl_pcm_info(handle, pcminfo)) < 0)
            {
                if (err != -ENOENT)
                    qDebug() << "control digital audio info" << card << ":" << snd_strerror(err);

                continue;
            }

            qDebug() << "Card" << card << ":"
                     << "[" << name << "," << dev << "]"
                     << snd_ctl_card_info_get_id(info)
                     << snd_ctl_card_info_get_name(info)
                     << dev << snd_pcm_info_get_id(pcminfo)
                     << snd_pcm_info_get_name(pcminfo);

            count = snd_pcm_info_get_subdevices_count(pcminfo);

            qDebug() << "  Subdevices:" << snd_pcm_info_get_subdevices_avail(pcminfo) << count;

            for (idx = 0; idx < (int)count; idx++)
            {
                snd_pcm_info_set_subdevice(pcminfo, idx);

                if ((err = snd_ctl_pcm_info(handle, pcminfo)) < 0)
                {
                    qDebug() << "Control digital audio playback info" << card << snd_strerror(err);
                }
                else
                {
                    qDebug() << "  Subdevice" << idx << snd_pcm_info_get_subdevice_name(pcminfo);
                }
            }
        }
        snd_ctl_close(handle);

        next_card:
        if (snd_card_next(&card) < 0)
        {
            qDebug() << "snd_card_next";
            break;
        }
    }
}

void printfmtmask(const snd_pcm_format_mask_t *fmask)
{
    int fmt, prevformat= 0;

    for( fmt= 0; fmt <= SND_PCM_FORMAT_LAST; ++fmt )
        if( snd_pcm_format_mask_test(fmask, (snd_pcm_format_t)fmt) )
        {
            qDebug() << snd_pcm_format_name((snd_pcm_format_t)fmt);
            prevformat= 1;
        }
    if( !prevformat )
        qDebug() << "(none)";
}

void info(char *dev_name, snd_pcm_stream_t stream)
{
    snd_pcm_hw_params_t *hw_params;
    int err;
    snd_pcm_t *handle;
    unsigned int max;
    unsigned int min;
    unsigned int val;
    int dir;
    snd_pcm_uframes_t frames;
    snd_pcm_format_mask_t *fmask;

    if ((err = snd_pcm_open (&handle, dev_name, stream, 0)) < 0)
    {
        qDebug() << "Cannot open audio device" << dev_name << snd_strerror (err);
        return;
    }

    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0)
    {
        qDebug() << "Cannot allocate hardware parameter structure" << snd_strerror (err);
        exit (1);
    }

    if ((err = snd_pcm_hw_params_any (handle, hw_params)) < 0)
    {
        qDebug() << "Cannot initialize hardware parameter structure" << snd_strerror (err);
        exit (1);
    }

    snd_pcm_format_mask_alloca(&fmask);
    snd_pcm_hw_params_get_format_mask(hw_params,fmask);
    printfmtmask(fmask);

    if ((err = snd_pcm_hw_params_get_channels_max(hw_params, &max)) < 0)
    {
    fprintf (stderr, "cannot  (%s)\n",
    snd_strerror (err));
    exit (1);
    }

    qDebug() << "Max channels" << max;

    if ((err = snd_pcm_hw_params_get_channels_min(hw_params, &min)) < 0)
    {
        qDebug() << "Cannot get channel info" << snd_strerror (err);
        exit (1);
    }

    qDebug() << "Min channels" << min;

    /*
    if ((err = snd_pcm_hw_params_get_sbits(hw_params)) < 0)
    {
        qDebug() << "Cannot get bits info" << snd_strerror (err);
        exit (1);
    }

    qDebug() << "Bits" << err;
    */

    if ((err = snd_pcm_hw_params_get_rate_min(hw_params, &val, &dir)) < 0)
    {
        qDebug() << "Cannot get min rate" << snd_strerror (err);
        exit (1);
    }

    qDebug() << "Min rate" << val;

    if ((err = snd_pcm_hw_params_get_rate_max(hw_params, &val, &dir)) < 0)
    {
        qDebug() << "Cannot get max rate" << snd_strerror (err);
        exit (1);
    }

    qDebug() << "Max rate" << val;

    if ((err = snd_pcm_hw_params_get_period_time_min(hw_params, &val, &dir)) < 0)
    {
        qDebug() << "Cannot get min period time" << snd_strerror (err);
        exit (1);
    }

    qDebug() << "Min period time (usecs)" << val;

    if ((err = snd_pcm_hw_params_get_period_time_max(hw_params, &val, &dir)) < 0)
    {
        qDebug() << "Cannot get max period time" << snd_strerror (err);
        exit (1);
    }

    qDebug() << "Max period time (usecs)" << val;

    if ((err = snd_pcm_hw_params_get_period_size_min(hw_params, &frames, &dir)) < 0)
    {
        qDebug() << "Cannot get min period size" << snd_strerror (err);
        exit (1);
    }

    qDebug() << "Min period size in frames" << (unsigned int)frames;

    if ((err = snd_pcm_hw_params_get_period_size_max(hw_params, &frames, &dir)) < 0)
    {
        qDebug() << "Cannot get max period size" << snd_strerror (err);
        exit (1);
    }

    qDebug() << "Max period size in frames" << (unsigned int)frames;

    if ((err = snd_pcm_hw_params_get_periods_min(hw_params, &val, &dir)) < 0)
    {
        qDebug() << "Cannot get min periods" << snd_strerror (err);
        exit (1);
    }

    qDebug() << "Min periods per buffers" << val;

    if ((err = snd_pcm_hw_params_get_periods_max(hw_params, &val, &dir)) < 0)
    {
        qDebug() << "Cannot get min periods" << snd_strerror (err);
        exit (1);
    }

    qDebug() << "Max periods per buffers" << val;

    if ((err = snd_pcm_hw_params_get_buffer_time_min(hw_params, &val, &dir)) < 0)
    {
        qDebug() << "Cannot get min buffer time" << snd_strerror (err);
        exit (1);
    }

    qDebug() << "Min buffer time (usecs)" << val;

    if ((err = snd_pcm_hw_params_get_buffer_time_max(hw_params, &val, &dir)) < 0) {
        qDebug() << "Cannot get max buffer time" << snd_strerror (err);
        exit (1);
    }

    qDebug() << "Max buffer time (usecs)" << val;

    if ((err = snd_pcm_hw_params_get_buffer_size_min(hw_params, &frames)) < 0)
    {
        qDebug() << "Cannot get min buffer size" << snd_strerror (err);
        exit (1);
    }

    qDebug() << "Min buffer size in frames" << (unsigned int)frames;

    if ((err = snd_pcm_hw_params_get_buffer_size_max(hw_params, &frames)) < 0)
    {
        qDebug() << "Cannot get max buffer size" << snd_strerror (err);
        exit (1);
    }

    qDebug() << "Max buffer size in frames" << (unsigned int)frames;

    snd_pcm_close(handle);

}


void listdev(char *devname)

{

    char** hints;
    int    err;
    char** n;
    char*  name;
    char*  desc;
    char*  ioid;

    /* Enumerate sound devices */
    err = snd_device_name_hint(-1, devname, (void***)&hints);
    if (err != 0)
    {
        qDebug() << "Cannot get device names\n";
        exit(1);
    }

    n = hints;
    while (*n != NULL)
    {
        name = snd_device_name_get_hint(*n, "NAME");
        desc = snd_device_name_get_hint(*n, "DESC");
        ioid = snd_device_name_get_hint(*n, "IOID");

        qDebug() << "\nName of device:" << name;
        qDebug() << "Description of device:" << desc;
        qDebug() << "I/O type of device:" << ioid;

        if (name && strcmp("null", name)) free(name);
        if (desc && strcmp("null", desc)) free(desc);
        if (ioid && strcmp("null", ioid)) free(ioid);
        n++;

    }

    //Free hint buffer too
    snd_device_name_free_hint((void**)hints);

}





#define BUFFER_SIZE 1024
void play()
{
    qDebug() << "int32_t:" << sizeof (int32_t) << ", int16_t:" << sizeof(int16_t);
    int err;
    int16_t buf[BUFFER_SIZE] = {0};

    snd_pcm_t *playback_handle;
    snd_pcm_hw_params_t *hw_params;
    FILE *fin;
    size_t nread;
    unsigned int rate = 48000;
    int32_t converter[BUFFER_SIZE] = {0};

    if ((err = snd_pcm_open (&playback_handle, "sysdefault:CARD=PCH", SND_PCM_STREAM_PLAYBACK, 0)) < 0)
    {
        qDebug() << "Cannot open audio device" << snd_strerror (err);
        exit (1);
    }

    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0)
    {
        qDebug() << "Cannot allocate hardware parameter structure" << snd_strerror (err);
        exit (1);
    }

    if ((err = snd_pcm_hw_params_any (playback_handle, hw_params)) < 0)
    {
        qDebug() << "Cannot initialize hardware parameter structure" << snd_strerror (err);
        exit (1);
    }

    if ((err = snd_pcm_hw_params_set_format (playback_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0)
    {
        qDebug() << "Cannot set sample format" << snd_strerror (err);
        exit (1);
    }

    /*
    snd_pcm_chmap_t *chmap = snd_pcm_get_chmap (playback_handle);
    char *chmapsInfo;
    snd_pcm_chmap_print (chmap,2000, chmapsInfo);
    qDebug() << chmapsInfo;
    */

    /*
    if ((err = snd_pcm_hw_params_set_periods(playback_handle,hw_params, 32, 0)) < 0 )
    {
        qDebug() << "Cannot set periods" << snd_strerror (err);
        exit (1);
    }
    */

    /*
    if((err = snd_pcm_hw_params_set_buffer_size(playback_handle,hw_params,BUFFER_SIZE*2)) < 0 )
    {
        qDebug() << "Cannot set buffer size" << snd_strerror (err);
        exit (1);
    }
    */

    if ((err = snd_pcm_hw_params_set_access (playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
    {
        qDebug() << "Cannot set access type" << snd_strerror (err);
        exit (1);
    }

    if ((err = snd_pcm_hw_params_set_rate_near (playback_handle, hw_params, &rate, 0)) < 0)
    {
        qDebug() << "Cannot set sample rate" << snd_strerror (err);
        exit (1);
    }

    qDebug() << "Rate set to" << rate;

    if ((err = snd_pcm_hw_params_set_channels (playback_handle, hw_params, 2)) < 0)
    {
        qDebug() << "Cannot set channel count" << snd_strerror (err);
        exit (1);
    }

    /*
    unsigned int channs = 0;
    snd_pcm_hw_params_get_channels(hw_params,&channs);
    qDebug() << channs << "CHANNELS";
    */

    if ((err = snd_pcm_hw_params (playback_handle, hw_params)) < 0)
    {
        qDebug() << "Cannot set parameters" << snd_strerror (err);
        exit (1);
    }

    snd_pcm_hw_params_free (hw_params);


    if ((err = snd_pcm_prepare (playback_handle)) < 0)
    {
        fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
        snd_strerror (err));
        exit (1);
    }


    if ((fin = fopen("/home/veronica/demo.wav", "r")) == NULL)
    {
        qDebug() << "Cannot open audio file.";
        exit(1);
    }


    unsigned int samplesToPlay = rate * 120;
    unsigned int samplesPlayed = 0;
    while ((nread = fread(buf, sizeof(int16_t), BUFFER_SIZE, fin)) > 0 && samplesPlayed < samplesToPlay)
    {


        // Transform stream format
        for(int i = 0; i < BUFFER_SIZE; i++)
        {
            converter[i] = buf[i]*10000;//32,767;
            //memcpy(&converter[i],&buf[i],2);
        }


        if ( snd_pcm_writei(playback_handle, buf, BUFFER_SIZE/2) < 0)
        {
            qDebug() << "Write to audio interface failed" << snd_strerror (err);
            snd_pcm_prepare(playback_handle);
        }

        samplesPlayed += nread;
    }
    snd_pcm_drain(playback_handle);
    snd_pcm_close (playback_handle);
    exit (0);
}

/*
    qDebug() << "Is little endian:" << isLittleEndian();

    listdev((char*)"pcm");
    //listdev((char*)"rawmidi");
    //device_list(SND_PCM_STREAM_CAPTURE);
    //device_list(SND_PCM_STREAM_PLAYBACK);
    //info((char *)"hw:0", SND_PCM_STREAM_CAPTURE);
    //info((char *)"hw:0", SND_PCM_STREAM_PLAYBACK);
    play();
*/


#endif // ALSAUTILS_H
