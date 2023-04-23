#include <stdio.h>
#include <stdlib.h>
#include <pulse/simple.h>
#include <unistd.h>

int main() {
    printf("hey!\n");
    
    pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .channels = 2,
        .rate = 44100
    };
    
    pa_simple *pa = pa_simple_new(NULL, "test", PA_STREAM_PLAYBACK, NULL, "Music", &ss, NULL, NULL, NULL);
    
    float buf[88200];
    while(1) {
        for(int i=0; i<88200; ++i) {
            buf[i] = (rand() * 2. - 1.) * .5;
        }
        pa_simple_write(pa, buf, 88200*sizeof(float), NULL);
        pa_simple_drain(pa, NULL);
    }
    
    pa_simple_free(pa);
    return 0;
}
