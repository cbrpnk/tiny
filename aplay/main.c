#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#define BUFFER_SIZE 4096
#define CHANNEL_COUNT 2

#define DELAY_LINE_SIZE 44100

#define MIN(a, b) (a) < (b) ? (a) : (b)
#define MAX(a, b) (a) > (b) ? (a) : (b)

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

float env(float time, float release) {
    return 1.0f/pow(2.0f, time*release);
}

/*
float m_fmod(float x, float y) {
    register long double value;
    __asm __volatile__(
        "1: fprem\n\t"
        "fnstsw    %%ax\n\t"
        "sahf\n\t"
        "jp    1b"
        : "=t" (value) : "0" (x), "u" (y) : "ax", "cc"
    );
    return value;
}
*/

float m_sin(float x) {
    float ret;
    asm volatile (
        "flds %1;"
        "fsin;"
        "fstps %0;"
        : "=m"(ret) : "m"(x)
    );
    return ret;
}

float osc(float time, float freq, float fm) {
    return m_sin(time * 2 * M_PI * freq + fm);
}

float modulo(float a, float modulus) {
    while(a > modulus) a -= modulus;
    return a;
}


float tan_h(float a) {
    return (2.0f / (1 + (1/pow(M_E, 2*a)))) - 1;
}

float noise() {
    return (float) rand() / RAND_MAX;
}

float drum(float time) {
    float sample = osc(time, 40, env(time, 300)*100);
    sample += noise() * env(time, 30.0f) * .3;
    sample += osc(time, 40, 0);
    return sample * env(time, 30.0f);
}

float last = 0.;
float lead(float time) {
    float f = 30 * (2 * (int)(modulo(time*2, 3))) * (int)(modulo(time*4+(int)(m_sin(time)), 4));
    time = modulo(time, .5);
    float mod3 = osc(time, f*120, last*.5) * .2;
    float mod2 = tan_h(osc(time, f*16, mod3)) * 3;
    float mod1 = osc(time, f*2, mod2) * 10;
    
    float pitch_env = 1.0f/pow(2.0f, time*10.0f);
    last = tanh(osc(time, 60, env(time, 10) + mod1) * env(time, 20) * 100.);
    return last;
}

float do_clap(float time) {
    return noise() * env(modulo(time+.5, 1.), 20);
}

float do_hihat(float time) {
    time = modulo(time+.25, .5);
    return noise() * env(time, 50);
}

void play(float time, float *left, float *right) {
    float kick = drum(modulo(time, .5));
    
    float l = lead(time) * .25;
    float d = delay_read(2);
    delay_write(l);
    l = -.5 *l + .2 * d;
    
    float clap = do_clap(time);
    float hihat = do_hihat(time);
    
    // no drums
    float lead_pass = MIN((int)(time/8), 1.);
    
    float mix = (kick + hihat + clap) * lead_pass + l;
    
    *left = mix;
    *right = mix;
}

void _start () {
    asm volatile("sub $8, %rsp\n");
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
    asm volatile(".intel_syntax noprefix");
	asm volatile("push 231"); //exit_group
	asm volatile("pop rax");
	// asm volatile("xor edi, edi");
	asm volatile("syscall");
	asm volatile(".att_syntax prefix");
	__builtin_unreachable();
}
