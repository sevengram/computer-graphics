#include "LiangBarsky.h"

/*
 *  Vector line clipping using the Liang-Barsky method
 *
 *  Note that this is C code using the C++ compiler to pass variables by reference
 */

//  Clipping window
extern double Xmin,Xmax,Ymin,Ymax;

/*
 *  Calculate t
 *    den is N(P0-P1)
 *    num is N(P0-Pe)
 *  For a window parallel to x and y axes:
 *    den is +/-dx or +/-dy
 *    num is the perpendicular distance to P0
 */
static int ClipT(double den,double num,double& t0,double& t1)
{
   //  Potential entry
   if (den>0)
   {
      double t = num/den;
      if (t>t1) return 0;  //  Crossover => reject
      if (t>t0) t0 = t;   //  New entry
   }
   //  Potential exit
   else if (den<0)
   {
      double t = num/den;
      if (t<t0) return 0;  //  Crossover => reject
      if (t<t1) t1 = t;   //  New exit
   }
   //  On outside edge => reject
   else if (num>0)
      return 0;
   //  Return Accept
   return 1;
}

/*
 *  Clip line
 *    -1 = Outside
 *     0 = Inside
 *     1,2 = Clipped
 */
int ClipLB(double& x0,double& y0,double& x1,double& y1)
{
   double dx = x1-x0;
   double dy = y1-y0;
   double t0 = 0;
   double t1 = 1;
   //  Check for degenerate lines
   if (dx==0 && dy==0)
      return (Xmin<=x0 && x0<=Xmax && Ymin<=y0 && y0<=Ymax) ? 0 : -1;
   //  Calculate t for all four edges
   else if (ClipT(+dx,Xmin-x0,t0,t1) &&  // Left
            ClipT(-dx,x0-Xmax,t0,t1) &&  // Right
            ClipT(+dy,Ymin-y0,t0,t1) &&  // Bottom
            ClipT(-dy,y0-Ymax,t0,t1))    // Top
   {
      int clip = 0;
      if (t1<1)
      {
         clip++;
         x1 = x0 + t1*dx;
         y1 = y0 + t1*dy;
      }
      if (t0>0)
      {
         clip++;
         x0 += t0*dx;
         y0 += t0*dy;
      }
      return clip;
   }
   else
     return -1;
}
