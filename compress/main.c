#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>
#include <portaudio.h>
#include <pa_linux_alsa.h>

#define MIN(a,b) (a)<(b) ? (a) : (b)
#define MAX(a,b) (a)>(b) ? (a) : (b)

uint64_t current_frame = 0;

void play(double time, float *left, float *right) {
    time = fmod(time, .5);
    
    float attack = pow(2, 1500*time) - 1;
    
    float lfo = sin(2 * M_PI * time)*10;
    float pitch_env = 1./(pow(2, 100.*time))*20;
    float env = 1./(pow(2, 30.*time));
    env = MIN(env, attack);
    float bass = sin(2 * M_PI * time * 60 +pitch_env) * env;
    
    // Limiter
    for(int i=0; i<3; ++i) {
        bass *= 1.3;
        bass = MIN(MAX(bass, -.8), .8);
    }
    
    *left = bass;
    *right = bass;
}

int callback(const void *input, void *output, unsigned long len, const PaStreamCallbackTimeInfo *time_info, 
                PaStreamCallbackFlags flags, void *user_data) {
    float *out = output;
    for(int i=0; i<len; ++i) {
        play((double) current_frame/44100, out, out+1);
        out += 2;
        current_frame++;
    }
    return 0;
}

int main() {
    srand(0);
    Pa_Initialize();
    
    PaStream *stream;
    Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, 44100, 1024, callback, NULL);
    Pa_StartStream(stream);
    for(;;) {
        sleep(1);
    }
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();
    return 0;
}
