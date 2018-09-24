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

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

int8_t rand_data[512];


int8_t test_data[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0xfe,
  0x02, 0xfc, 0x04, 0xf8, 0x0a, 0xf1, 0x14, 0xe2, 0x2d, 0xa9, 0x7f, 0x7f, 0x7f,
  0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
  0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0xff, 0x80, 0x80, 0x80,
  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x7f, 0x7f, 0x7f, 0x7f,
  0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
  0x00, 0xfe, 0xff, 0xf3, 0xf3, 0xec, 0xdd, 0xe3, 0xec, 0xd3, 0x25, 0xfb, 0x3c,
  0x7f, 0x3d, 0x7f, 0x7f, 0x7f, 0x7f, 0x31, 0xd3, 0x24, 0x80, 0x81, 0x80, 0x80,
  0x80, 0x80, 0x80, 0xaf, 0xa5, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x5b,
  0x7f, 0xbb, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x7f, 0xf0,
  0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0xb5, 0x43, 0xc7, 0x80, 0x80, 0x80,
  0x80, 0x80, 0x80, 0x80, 0x4a, 0x80, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
  0x53, 0x63, 0x29, 0x80, 0x01, 0x80, 0x8b, 0x93, 0x80, 0xc9, 0xcb, 0xda, 0x10,
  0x08, 0x0f, 0x26, 0x07, 0x16, 0x07, 0x03, 0x02, 0x00 };

static double filt[2048];


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
double eval_fir(double *coef, double next_sample, int len, int do_rounding) {
int i;
double val = 0.0;

  //evaluate fir for the new sample
  for(i=len-1;i>=0;i--) {
    if(do_rounding) {
      val += round(filt[i]) * round(coef[i]); 
    }
    else {
      val += filt[i] * coef[i]; 
    }
  }

  //shift sample buffer
  for(i=0;i<len-1;i++) {
    filt[i] = filt[i+1];
  }
  //move next sample in
  filt[len-1] = next_sample; 


  
  return val;
}
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
void test_filter(double *coef, int len, int do_rounding) {
  struct timespec requestStart;
  clock_gettime(CLOCK_REALTIME, &requestStart);

  int i;
  srand(requestStart.tv_nsec);
  for(i=0;i<sizeof(rand_data);i++) {
    rand_data[i] = (int8_t) (rand()%256);
  }

  //test data no filter
  printf("\r\nstart_test_data\r\n");
  for(i=0;i<sizeof(test_data);i++) {
    printf("%d\r\n",test_data[i]);
  }
  for(i=0;i<sizeof(test_data);i++) {
    printf("%d\r\n",test_data[i]);
  }
  for(i=0;i<sizeof(rand_data);i++) {
    printf("%d\r\n",rand_data[i]);
  }
  printf("\r\nend_test_data\r\n");

  //test data with filter
  printf("\r\nstart_test_filter\r\n");
  for(i=0;i<sizeof(test_data);i++) {
    double d = eval_fir(coef,(double) test_data[i],len, do_rounding);
    printf("%f\r\n", d); 
  }
  for(i=0;i<sizeof(test_data);i++) {
    double d = eval_fir(coef,(double) test_data[i],len, do_rounding);
    printf("%f\r\n", d); 
  }
  for(i=0;i<sizeof(rand_data);i++) {
    double d = eval_fir(coef,(double) rand_data[i],len, do_rounding);
    printf("%f\r\n", d); 
  }
  printf("\r\nend_test_filter\r\n");
}
