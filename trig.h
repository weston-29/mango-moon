#ifndef TRIG_H
#define TRIG_H

// trig.h defines trig functions to create a sin and cos lookup table


#define PI 3.14159265358979323846

// table of sin values from 0 to 359 that can be used in other files
// when finding sin/cos of an angle, always perform angle %= 360 before using as index
extern double sin_table[360];
extern double cos_table[360]; 

// factorial of n
double factorial(int n);

// x to the power of n
double power(double x, int n);

// Find sin using Taylor series
double sin(double x);

// Find cos using Taylor series
double cos(double x);

// find floating point absolute value
double fabs(double x);

// populate above sin/cos_tables
void generate_tables(void);

#endif
