#ifndef __BarnesHut__constants__
#define __BarnesHut__constants__

#define G 6.673e-11

#define EPS 1.0e1 // Smoothening.
#define EPS2 EPS*EPS

#define THETA 1.0/2.0 // Divided by 2 due to radius instead of diameter.
#define RADIUS_FACTOR 10.0 // xN times bigger cube than minimum.
#define BOUNCE_FACTOR 0.02

// days per year * hours per day * minutes per hour * seconds per minute
#define YEAR 365.25 * 24.0 * 60.0 * 60.0 // seconds


// Slow:
#define TIME_STEP 100.0
#define OUTPUT_STEP TIME_STEP * 200.0

// Fast:
//#define TIME_STEP 10000.0
//#define OUTPUT_STEP TIME_STEP * 2000.0

#define X 0
#define Y 1
#define Z 2

#define TIME_NO

#endif