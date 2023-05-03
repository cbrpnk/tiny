// Taken from math.h
#define M_E   2.7182818284590452354
#define M_PI  3.14159265358979323846

#define BUFFER_SIZE 4096
#define CHANNEL_COUNT 2

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

float m_exp(float a) {
    float res = 1;
    int n = 100;
    for(int i=0; i<n; ++i) {
        res *= (1 + a/n);
    }
    return res;
}

float env(float time, float release) {
    return 1.0f/m_exp(time*release);
}

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
    return (2.0f / (1 + (1/m_exp(2*a)))) - 1;
}

int seed = 1;
float noise() {
    int res = (seed = (48271*seed) % 2147483647);
    return (float) res / (4294967295U);
}

float drum(float time) {
    float sample = osc(time, 40, env(time, 300)*100);
    sample += noise() * env(time, 30.0f) * .3;
    sample += osc(time, 40, 0);
    return sample * env(time, 30.0f);
}

float last = 0.;
float do_bass(float time) {
    float t = time;
    float f = 200 * (int)(modulo(time*2+(int)(m_sin(time)), 2)) * m_sin(t*.1);
    //time = modulo(time, MAX(.8, m_sin(t)*2.));
    time = modulo(time, MAX(.2, m_sin(tan_h(time*4))*.5)*.5);
    float mod3 = osc(time, f*2, last*.2) * .1;
    float mod2 = tan_h(osc(time, f*2, mod3)*.15) * m_sin(t);
    float mod1 = osc(time, f*2, mod2) * 8 * m_sin(t*8+M_PI);
    
    last = tan_h(osc(time, 50, env(time, 10) * mod1) * env(time, 25) * 30.);
    return last;
}

float do_lead(float time) {
    time += tan_h(((m_sin(time*32)*.25+1)*.1))*.1 + 1;
    float o1 = osc(time, 1000, 0);
    float o2 = osc(time, 3000, o1*20);
    float o3 = osc(time, 4000, o2);
    return (o1*.2 + o2*.2 + o3*.2) * env(modulo(time, .16), 25);
}

float do_clap(float time) {
    return tan_h(noise() * env(modulo(time+.5, 1.), 20) * 2.);
}

float do_hihat(float time) {
    time = modulo(time+.25, .5);
    return noise() * env(time, 50);
}

void play(float time, float *left, float *right) {
    time += tan_h(((m_sin(time*16)*.5+.5)*.03))*.2 + 1;
    time = modulo(time, 8)+1;
    float kick = drum(modulo(time, .5));
    float bass = do_bass(time) * .25;
    
    
    float lead = do_lead(time) * .25;
    float clap = do_clap(time);
    float hihat = do_hihat(time);
    
    float intro = time > 8;
    
    float mix = (kick + bass + hihat + lead);
    
    *left = mix;
    *right = mix;
}

void _start () {
    asm volatile("sub $8, %rsp\n");
    int frame_id = 0;
    float buffer[BUFFER_SIZE*CHANNEL_COUNT];
    float *buf = &buffer[0];
    while(1) {
        for(int i=0; i<BUFFER_SIZE*CHANNEL_COUNT; i+=2) {
            float time = (float) frame_id / 44100;
            play(time, buffer+i, buffer+i+1);
            frame_id++;
        }
        asm volatile (
            "mov $1, %%rax;"
            "mov $1, %%rdi;"
            "mov %0, %%rsi;"
            "mov %1, %%rdx;"
            "syscall;"
            :: "m"(buf), "i"(sizeof(float) * BUFFER_SIZE * CHANNEL_COUNT)
        );
    }
    asm volatile(".intel_syntax noprefix");
	asm volatile("push 231"); //exit_group
	asm volatile("pop rax");
	// asm volatile("xor edi, edi");
	asm volatile("syscall");
	asm volatile(".att_syntax prefix");
	__builtin_unreachable();
}
