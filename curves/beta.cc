#include <math.h>
#include <stdio.h>

static double velocity(double p, double m)
{
  return p/sqrt(m*m + p*p);
} /* velocity */

#define _MPI_      (0.140)
#define _MK_       (0.494)
#define _MP_       (0.938)
#define _ME_       (0.0005)

// Light velocity --> mm/ns
#define _LIGHT_VELOCITY_     299.79

int main( void )
{
  double m1 = _ME_, m2 = _MPI_, p = 1.0, l = 1200.;
  double v1 = velocity(p, m1), v2 = velocity(p, m2);
  double t1 = l / (_LIGHT_VELOCITY_*v1), t2 = l / (_LIGHT_VELOCITY_*v2);

  printf("%f %f -> %f %f -> %5.2f\n", v1, v2, t1, t2, 1000*(t2-t1)); 
} // main()
