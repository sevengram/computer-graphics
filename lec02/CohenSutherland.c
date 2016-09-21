#include "CohenSutherland.h"

/*
 *  Vector line clipping using the Cohen-Sutherland method
 *
 *  Note that this is C code using the C++ compiler to pass variables by reference
 */

//  Position bits
enum {TOP=0x01,BOTTOM=0x02,RIGHT=0x04,LEFT=0x08};

//  Clipping window
extern double Xmin,Xmax,Ymin,Ymax;

/*
 *  Calculate clipping code
 */
static int ClipCode(double x,double y)
{
   int code=0;
   if (x>Xmax) code |= RIGHT;
   if (x<Xmin) code |= LEFT;
   if (y>Ymax) code |= TOP;
   if (y<Ymin) code |= BOTTOM;
   return code;
}

/*
 *  Clip line according to clipping code
 *  Return clip code and set (x,y)
 */
static int ClipLine(int code,double x0,double y0,double x1,double y1,
                    double& x,double& y)
{
   //  Clip against top or bottom edge
   if (code&TOP || code&BOTTOM)
   {
      //  Set y to top or bottom
      y = code&TOP ? Ymax : Ymin;
      //  Calculate x intersection
      x = x0 + (x1-x0)*(y-y0)/(y1-y0);
      //  Update clip code
      if (x<Xmin)
        code = LEFT;
      else if (x>Xmax)
        code = RIGHT;
      else
        code = 0;
   }
   //  Clip against right or left edge
   if (code&RIGHT || code&LEFT)
   {
      //  Set x to right or left
      x = code&RIGHT ? Xmax : Xmin;
      //  Calculate y intersection
      y = y0 + (y1-y0)*(x-x0)/(x1-x0);
      //  Update clip code
      if (y<Ymin)
        code = BOTTOM;
      else if (y>Ymax)
        code = TOP;
      else
        code = 0;
   }
   return code;
}

/*
 *  Clip line
 *    -1 = Outside
 *     0 = Inside
 *     1,2 = Clipped
 */
int ClipCS(double& x0,double& y0,double& x1,double& y1)
{
   //  Get clip codes for (x0,y0) and (x1,y1)
   int code0 = ClipCode(x0,y0);
   int code1 = ClipCode(x1,y1);
   //  Both inside - no clipping required
   if (!(code0|code1))
      return 0;
   //  Both outside and cannot cross window - do nothing
   else if (code0&code1)
      return -1;
   //  Clip those ends that are outside the window
   else if ((code0 && ClipLine(code0,x0,y0,x1,y1,x0,y0)) ||
            (code1 && ClipLine(code1,x0,y0,x1,y1,x1,y1)))
      return -1;
   else
      return (code0?1:0) + (code1?1:0);
}
