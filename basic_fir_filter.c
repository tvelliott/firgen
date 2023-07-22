
//MIT License
//
//Copyright (c) 2017 tvelliott
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.
//
//
//initial inspiration and sinc, dft code from here:  basic_fir_design.c : design basic FIR filter from liquid-dsp.org blog
//most of the windows are described here:  https://en.wikipedia.org/wiki/Window_function
//calc_bessel function from here: https://github.com/Cognoscan,  FilterBuilder 

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <unistd.h>

void test_filter(double *coef, int len, int do_rounding);
float sincf(float x);

double calc_bessel(double x)
{
double ax;
double y;

  ax = fabs(x);
  if (ax < 3.75) {
    y = x/3.75;
    y = y*y;
    return 1.0+y*(3.5156229+y*(3.0899424+y*(1.2067492 +y*(0.2659732+y*(0.360768e-1+y*0.45813e-2)))));
  }
  else {
    y = 3.75/ax;
    return (exp(ax)/sqrt(ax))*(0.39894228+y*(0.1328592e-1 +y*(0.225319e-2+y*(-0.157565e-2+y*(0.916281e-2 +y*(-0.2057706e-1+y*(0.2635537e-1)))))));
  }
}

int main( int argc, char **argv ) {
// parameters and simulation options
unsigned int h_len =   57;  // filter length (samples)
float        fc    = 0.20;  // normalized cutoff frequency
unsigned int nfft  =  2000;  // 'FFT' size (actually DFT)

unsigned int i;
unsigned int k;
int c;

int win_type=0;
int data_type=0;
int filt_len=57;
float bw_type=0.2;
int code_type=0;
double gain = 1.0f;
float alpha = 0.5;
double denom = 1.0;
int do_rounding=0;
double sinc_gain=0.0;

double a0;
double a1;
double a2;
double a3;
double a4;

  //if(argc>0) printf("%s", argv[1]);


  
  while ((c = getopt(argc, argv, "w:d:l:b:c:g:a:s:")) != -1) {
    switch (c) {
      case 'w': 
        win_type = atoi( optarg ); 
        //printf("\r\nwin_type: %d", win_type);
      break;

      case 'd': 
        data_type = atoi( optarg ); 
      break;

      case 'l': 
        filt_len = atoi( optarg ); 
      break;

      case 'b': 
        bw_type = atof( optarg ); 
        bw_type /= 2.0;
      break;

      case 'c': 
        code_type = atoi( optarg ); 
      break;

      case 'g': 
        gain = (double) atof( optarg ); 
      break;

      case 'a': 
        alpha = (double) atof( optarg ); 
      break;

      case 's': 
        sinc_gain = (double) atof( optarg ); 
      break;
    }
  }

  h_len = filt_len;
  fc = bw_type;


  switch( data_type ) { 
    case 0 :
      printf("\r\ndouble coef[] = {\n");
      do_rounding=0;
    break;
    case 1 :
      printf("\r\nfloat coef[] = {\n");
      do_rounding=0;
    break;
    case 2 :
      printf("\r\nint32_t coef[] = {\n");
      do_rounding=1;
    break;
    case 3 :
      printf("\r\nint16_t coef[] = {\n");
      do_rounding=1;
    break;
    case 4 :
      printf("\r\nint8_t coef[] = {\n");
      do_rounding=1;
    break;

    default :
      printf("\r\ndouble coef[] = {\n");
      do_rounding=0;
  }

  // design filter
  double h[h_len];
  double s[h_len];
  double w[h_len];


  for (i=0; i < h_len; i++) {
    if(i>0) printf(", ");

    // generate time vector, centered at zero
    float t = (float)i + 0.5f - 0.5f*(float)h_len;

    // generate sinc function (time offset in 't' prevents divide by zero)
    s[i] = sinf(2*M_PI*fc*t + 1e-6f) / (2*M_PI*fc*t + 1e-6f);

    w[i] = 1.0;  //rectangular
    
    switch(win_type) {
      case 0 :
        // generate Hamming window
        w[i] = 0.53836 - 0.46164*cosf((2*M_PI*(float)i)/((float)(h_len-1)));
      break;

      case 1 :
        // generate blackman 
        // generate blackman / nutall
        a0 = 0.42659;
        a1 = 0.49656;
        a2 = 0.076849;
        w[i] = a0 - a1*cos(2*M_PI*(float)i/(h_len-1)) + 
                 a2*cos(4*M_PI*(float)i/(h_len-1));
      break;

      case 2 :
        // generate kaiser 
        denom = calc_bessel(M_PI*alpha);
        w[i] = calc_bessel(M_PI*alpha*sqrt(1-pow((2*(float)i/(h_len-1))-1,2))) / denom;
      break;

      //Hann window / Raised Cosine
      case 3 :
         w[i] = 0.5 - 0.5*cos(2*M_PI*(float)i/(h_len-1));
      break;

      case 4 :
        // generate blackman / nutall
        a0 = 0.3635819;
        a1 = 0.4891775; 
        a2 = 0.1365995; 
        a3 = 0.0106411; 
        w[i] = a0 - a1*cos(2*M_PI*(float)i/(h_len-1)) + 
                 a2*cos(4*M_PI*(float)i/(h_len-1)) - 
                 a3*cos(6*M_PI*(float)i/(h_len-1));
      break;

      case 5 :
        // generate blackman / harris
        a0 = 0.35875;
        a1 = 0.48829;
        a2 = 0.14128;
        a3 = 0.01168;
        w[i] = a0 - a1*cos(2*M_PI*(float)i/(h_len-1)) + 
                 a2*cos(4*M_PI*(float)i/(h_len-1)) - 
                 a3*cos(6*M_PI*(float)i/(h_len-1));
      break;

      case 6 :
        // flat-top 
        a0 = 1.0; 
        a1 = 1.93; 
        a2 = 1.29; 
        a3 = 0.388; 
        a4 = 0.028; 
        w[i] = a0 - a1*cos(2*M_PI*(float)i/(h_len-1)) + 
                 a2*cos(4*M_PI*(float)i/(h_len-1)) - 
                 a3*cos(6*M_PI*(float)i/(h_len-1)) +
                 a3*cos(8*M_PI*(float)i/(h_len-1));
      break;

      case 7 :
        // triangle 
        w[i] = 1.0 - fabs( ((float) i - ((float) h_len - 1) / 2.0) / ((float) h_len / 2.0) );
      break;

      case 8 :
        // sine window 
        w[i] = sin( M_PI*(float)i / ((float)h_len-1) );
      break;

      case 9 :
        //  welch window
        w[i] = 1.0 - pow(  ((float)i - ((float)h_len-1.0)/2.0) / ( ((float)h_len-1.0)/2.0 )  ,2.0); 
      break;

      case 10 :
        //  gaussian window 
        if(alpha > 0.5) alpha = 0.5;
        a0 = (float) i - ((float) h_len - 1.0) / 2.0;
        a1 = alpha * ((float) h_len - 1.0) / 2.0;
        w[i] = cexpf( pow((a0/a1),2.0)*-0.5 );
      break;

      case 11 :
          //rectangular - do nothing
      break;
    }

    // generate composite filter coefficient
    h[i] = s[i] * w[i] * gain;


    switch( data_type ) { 
      case 0 :
        printf("%12.8f", (double) h[i] );
      break;
      case 1 :
        printf("%4.5f", (float) h[i] );
      break;
      case 2 :
        h[i] *= pow(2,32-1)-1;
        h[i] = round( h[i] );
        printf("%d", (int32_t) h[i] );
      break;
      case 3 :
        h[i] *= pow(2,16-1)-1;
        h[i] = round( h[i] );
        printf("%d", (int16_t) h[i] );
      break;
      case 4 :
        h[i] *= pow(2,8-1)-1;
        h[i] = round( h[i] );
        printf("%d", (int8_t) h[i] );
      break;
    }



    if(i>0 && i%16==0) printf("\r\n");
  }


  printf(" };\r\n//start_mag\r\n");

  // run filter analysis with discrete Fourier transform

  for (i=0; i < nfft; i++) {
    // accumulate energy in frequency (also apply frequency shift)
    float complex H = 0.0f;
    float frequency = (float)(i+nfft/2)/((float)nfft);
    for (k=0; k < h_len; k++)
    H += h[k]*cexpf(_Complex_I*2*M_PI*k*frequency);

    // print resulting power spectral density to standard output
    printf("%12.8f\n", 20*log10(cabsf(H*2*fc)));
  }
  printf(" };\r\n//end_mag\r\n");

  test_filter( h, h_len, do_rounding );

  output_verilog(argc, argv, s, w, h, h_len, 1.0);

  printf("\r\n");
  for (i=0; i < h_len; i++) {
    float idx = (float) i - (float) h_len/2.0;
    if(sinc_gain>0) {
      h[i] *= sinc_gain * pow((idx),2.0); 
      printf("%f, ", h[i]);
    }
  }



  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// compute sinc(x) = sin(pi*x) / (pi*x)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float sincf(float _x) {
    // _x ~ 0 approximation
    //if (fabsf(_x) < 0.01f)
    //    return expf(-lngammaf(1+_x) - lngammaf(1-_x));

    // _x ~ 0 approximation
    // from : http://mathworld.wolfram.com/SincFunction.html
    // sinc(z) = \prod_{k=1}^{\infty} { cos(\pi z / 2^k) }
    if (fabsf(_x) < 0.01f)
        return cosf(M_PI*_x/2.0f)*cosf(M_PI*_x/4.0f)*cosf(M_PI*_x/8.0f);

    return sinf(M_PI*_x)/(M_PI*_x);
}

