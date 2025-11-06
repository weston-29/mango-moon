// trig.c defines trig functions to create a sin and cos lookup table

#include "trig.h"


// table of sin values from 0 to 359 that can be used in other files (externally defined in .h)
// when finding sin/cos of an angle, always perform angle %= 360 before using as index
double sin_table[360];
double cos_table[360];

// factorial of n
double factorial(int n) {
   double result = 1.0;
   for (int i = 2; i <= n; i++) {
      result *= i;
   }
   return result;
}

// x to the power of n
double power(double x, int n) {
   double result = 1.0;
   for (int i = 0; i < n; i++) {
      result *= x;
   }
   return result;
}

// returns absolute value of a given floating point
// used to offset floating point precision errors when testing values
double fabs(double x) {
   if (x < 0) {
      return -x;
   }
   else {
      return x;
   }
}

// Find sin using Taylor series of any angle in radians
double sin(double x) {
   // normalize angle if greater than 2pi to be within 0-2pi range
   // (you can't use modulo on doubles, so we have to do this method)
   while (x >= (2 * PI)) {
      x -= (2 * PI);
   }
   // check for negative degrees
   while (x < 0) {
      x += (2 * PI);
   }
   
   double result = 0.0;
   int plusminus = 1; // switches between minus and plus in taylors series
		     
    for (int i = 0; i < 8; i++) {
       int exp = 2 * i + 1;
       result += plusminus * power(x, exp) / factorial(exp);
       plusminus *= -1;
    }

    return result;
} 

// Find cos using Taylor series of any angle in radians
double cos(double x) {
    // implementation is the same as sin except the exponent starts at 0 instead of 1
    while (x >= (2 * PI)) {
       x -= (2 * PI);
    }
    while (x < 0) {
       x += (2 * PI);
    }

    double result = 0.0;
    int plusminus = 1;
    for (int i = 0; i < 8; i++) {
       int exp = 2 * i;
       result += plusminus * power(x, exp) / factorial(exp);
       plusminus *= -1;
    }

    return result;

}


// populate above sin/cos_tables
void generate_tables(void) {
    // define each value in sin_table and cos_table to corresponding value (0->359)
    for (int i = 0; i < 360; i++) {
        // convert degrees to rad
        double rad = i * (PI/180.0);
        sin_table[i] = sin(rad);
        cos_table[i] = cos(rad);	
    }
}

