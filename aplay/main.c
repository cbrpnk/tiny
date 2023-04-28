#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#define BUFFER_SIZE 4096
#define CHANNEL_COUNT 2

#define DELAY_LINE_SIZE 44100
float delay_buf[DELAY_LINE_SIZE] = { 0 };
int delay_ptr = 0;

void delay_write(float v) {
    delay_buf[delay_ptr++] = v;
    delay_ptr %= DELAY_LINE_SIZE;
}

float delay_read(int i) {
    int pos = 0;
    if(delay_ptr < i) {
        pos = (i - delay_ptr);
        pos = DELAY_LINE_SIZE - pos - 1;
    } else {
        pos = delay_ptr - i;
    }
    return delay_buf[delay_ptr];
}


float drum(float time) {
    float noise = (float) rand() / RAND_MAX;
    float pitch_env = 1.0f/pow(2.0f, time*300.0f) * 100.;
    float sample = sin(time * 2 * M_PI * 40 + pitch_env);
    float env = 1.0f/pow(2.0f, time*30.0f);
    float noise_env = 1.0f/pow(2.0f, time*200.0f);
    sample += noise * noise_env * .3;
    float sub = sin(time * 2 * M_PI * 40);
    sample += sub;
    return sample * env;
}

float last = 0.;
float hihat(float time) {
    float f = 30 * floor(fmod(time, 4));
    time = fmod(time, .5);
    float mod3 = sin(time * 2 * M_PI * f*100 + last*.5) * 1.;
    float mod2 = tanh(sin(time * 2 * M_PI * f*16 + mod3) * 3.);
    float mod1 = sin(time * 2 * M_PI * f*2 + mod2) * 10.;
    
    //float noise = (float) rand() / RAND_MAX;
    float pitch_env = 1.0f/pow(2.0f, time*10.0f);
    float noise = sin(time * 2 * M_PI * 60 + pitch_env + mod1);
    float env = 1.0f/pow(2.0f, time*20.0f);
    last = tanh(noise * env * 100.);
    return last;
}

void play(float time, float *left, float *right) {
    float kick = drum(fmod(time, .5));
    
    float bass = hihat(time) * .25;
    float d = delay_read(2);
    delay_write(bass);
    bass = -.5 * bass + .2 * d;
    
    float mix = kick + bass;
    
    *left = mix;
    *right = mix;
}

int main() {
    srand(0);
    int frame_id = 0;
    float buffer[BUFFER_SIZE*CHANNEL_COUNT];
    while(1) {
        for(int i=0; i<BUFFER_SIZE*CHANNEL_COUNT; i+=2) {
            float time = (float) frame_id / 44100;
            play(time, buffer+i, buffer+i+1);
            frame_id++;
        }
        write(1, &buffer, sizeof(float) * BUFFER_SIZE*CHANNEL_COUNT);
    }
    return 0;
}
