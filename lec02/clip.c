#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "CohenSutherland.h"
#include "LiangBarsky.h"

/*
 *  Vector line clipping test program
 *
 *  Note that this is C code using the C++ compiler to pass variables by reference
 */

//  Global clip window
double Xmin,Xmax,Ymin,Ymax;

/*
 *  Test clipping using Cohen-Sutherland and Liang-Barsky
 */
int main(int argc,char* argv[])
{
   int i;

   //  Check usage
   if (argc!=5 && argc!=9)
   {
      fprintf(stderr,"Usage: %s x0 y0 x1 y1 [Xmin Xmax Ymin Ymax]\n",argv[0]);
      return 1;
   }

   // Clip window
   Xmin = (argc==9) ? strtod(argv[5],NULL) : 0;
   Xmax = (argc==9) ? strtod(argv[6],NULL) : 1;
   Ymin = (argc==9) ? strtod(argv[7],NULL) : 0;
   Ymax = (argc==9) ? strtod(argv[8],NULL) : 1;

   //  Loop for clipping methods
   for (i=0;i<2;i++)
   {
      //  Get (x0,y0) and (x1,y1)
      double x0 = strtod(argv[1],NULL);
      double y0 = strtod(argv[2],NULL);
      double x1 = strtod(argv[3],NULL);
      double y1 = strtod(argv[4],NULL);
      //  Clip using one or the other method
      int k = i ? ClipCS(x0,y0,x1,y1) : ClipLB(x0,y0,x1,y1);
      //  Print results
      if (k<0)
         printf("Outside\n");
      else
         printf("%s %d  %f %f   %f %f\n",i?"CS":"LB",k,x0,y0,x1,y1);
   }
   return 0;
}
