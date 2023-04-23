#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alsa/asoundlib.h>

int main() {
    // Select card
    int card_idx = -1;
    for(;;) {
        snd_card_next(&card_idx);
        if(card_idx != -1) break;
    }
    
    char card_name[32];
    snprintf(card_name, sizeof(card_name), "hw:%u", card_idx);
    
    snd_ctl_t *snd_ctl = NULL;
    snd_ctl_open(&snd_ctl, card_name, 0);
    
    // Select device
    int dev_idx = -1;
    for(;;) {
        if(snd_ctl_pcm_next_device(snd_ctl, &dev_idx) != 0 || dev_idx != -1) break;
    }
    char dev_name[64];
    snprintf(dev_name, sizeof(dev_name), "plughw:%u,%u", card_idx, dev_idx);
    printf("%s\n", dev_name);
    
    // Open audio buffer
    snd_pcm_t *pcm;
    snd_pcm_open(&pcm, dev_name, SND_PCM_STREAM_PLAYBACK, 0);
    
    
    snd_pcm_close(pcm);
    snd_ctl_close(snd_ctl);
    return 0;
}
