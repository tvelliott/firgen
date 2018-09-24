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

#include <stdio.h>

/////////////////////////////////////////////////
// verilog filter generator
// @author t.elliott 2017
/////////////////////////////////////////////////
void output_verilog(int argc, char *args[], double *s, double *w, double *coef, int length, double gain) {
int i;

printf("\r\n//verilog filter generator");
printf("\r\n//filter auto-generated: "); 

printf("\r\n//sinc function\r\n//");
for(i=0; i<length; i++) {
  printf("%f,", s[i]); 
}
printf("\r\n//window function\r\n//");
for(i=0; i<length; i++) {
  printf("%f,", w[i]); 
}

printf("\r\n//filter coeff\r\n//");
for(i=0; i<length; i++) {
  printf("%f,", coef[i]); 
}

printf("\r\n//");
for(i=0; i<argc; i++) {
  printf(" %s", args[i]);
}
printf("\r\n");
printf("\r\nmodule fir_filter8_16 (");
printf("\r\n  input clk,");
printf("\r\n  input clk_en,");
printf("\r\n  input rst,");
printf("\r\n  input signed [7:0] filter_in,");
printf("\r\n  output signed [16:0] filter_out17,");
printf("\r\n  input filter_bypass");
printf("\r\n);");
printf("\r\n");
printf("\r\nreg signed [16:0] outputreg;");
printf("\r\n");

for(i=0;i<length;i++) {
  printf("\r\n  reg signed [7:0] coef%d;", i+1);
}
printf("\r\n");

for(i=0;i<length;i++) {
  printf("\r\n  reg signed [7:0] fifo_%d;", i);
}
printf("\r\n");

printf("\r\n  reg signed [24:0] sum;");
printf("\r\n");
printf("\r\n  always @( posedge clk)");
printf("\r\n    begin");
printf("\r\n      if (!rst) begin");
printf("\r\n");

for(i=0;i<length;i++) {
  printf("\r\n        fifo_%d <= 0;",i);
}
printf("\r\n");

for(i=0;i<length;i++) {
  printf("\r\n        coef%d <= %d;",i+1, (int)(coef[i]*gain));
}

printf("\r\n");
printf("\r\n         sum <= 0;");
printf("\r\n      end");
printf("\r\n      else begin");
printf("\r\n        //if (clk_en== 1'b1) begin");
printf("\r\n          //");
printf("\r\n          sum <= (fifo_0 * coef1) +");

for(i=1;i<length-1;i++) {
  printf("\r\n               (fifo_%d * coef%d) +", i, i+1);
}
printf("\r\n               (fifo_%d * coef%d);", i, i+1);

printf("\r\n");
printf("\r\n      if(!filter_bypass) begin");
printf("\r\n          outputreg <= sum;");
printf("\r\n      end else begin");
printf("\r\n          outputreg <= filter_in*128;");
printf("\r\n      end");
printf("\r\n");

for(i=0;i<length-1;i++) {
  printf("\r\n            fifo_%d <= fifo_%d;", i, i+1);
}
printf("\r\n          fifo_%d <= filter_in;", length-1);

printf("\r\n");

printf("\r\n        //end");
printf("\r\n      end");
printf("\r\n    end");
printf("\r\n");
printf("\r\n");
printf("\r\n    assign filter_out17 = outputreg;");
printf("\r\n");
printf("\r\nendmodule");
printf("\r\n");

};
