//This file is part of EESLISM.
//
//Foobar is free software : you can redistribute itand /or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//Foobar is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with Foobar.If not, see < https://www.gnu.org/licenses/>.

#include <stdio.h>
#include <math.h>
#include "MODEL.h"


///*
//
//                         法線ベクトルを求める
//                                        FILE=HOUSEN.c
//                                        Create Date=1998.10.26
//                                        Update 2007.10.11 higuchi
//
//*/

void HOUSEN(int w,P_MENN *LP)
{

int i ;
double x,x1,y,y1,z,z1,el ;



 for (i=0 ;i<w ;i++){

    x=LP[i].P[1].X-LP[i].P[0].X ;
    y=LP[i].P[1].Y-LP[i].P[0].Y ;
    z=LP[i].P[1].Z-LP[i].P[0].Z ;
    x1=LP[i].P[2].X-LP[i].P[0].X ;
    y1=LP[i].P[2].Y-LP[i].P[0].Y ;
    z1=LP[i].P[2].Z-LP[i].P[0].Z ;
  
    LP[i].e.X = y * z1 - z * y1 ;
    LP[i].e.Y = z * x1 - x * z1 ;
    LP[i].e.Z = x * y1 - y * x1 ;

    el = sqrt(LP[i].e.X * LP[i].e.X + LP[i].e.Y * LP[i].e.Y + LP[i].e.Z * LP[i].e.Z) ;
    LP[i].e.X = LP[i].e.X / el ;
    LP[i].e.Y = LP[i].e.Y / el ;
    LP[i].e.Z = LP[i].e.Z / el ;
    /*--
 printf("LP[%d] X=%f,Y=%f,Z=%f\n",i,LP[i].e.X,LP[i].e.Y,LP[i].e.Z) ;
 --*/
 } 

}

void HOUSEN2(XYZ *p0, XYZ *p1, XYZ *p2, XYZ *e)
{
double x,x1,y,y1,z,z1,el ;

      x = p1->X - p0->X ;
      y = p1->Y - p0->Y ;
      z = p1->Z - p0->Z ;
      x1 = p2->X - p0->X ;
      y1 = p2->Y - p0->Y ;
      z1 = p2->Z - p0->Z ;
      e->X = y * z1 - z * y1 ;
      e->Y = z * x1 - x * z1 ;
      e->Z = x * y1 - y * x1 ;
      el = sqrt(e->X * e->X + e->Y * e->Y + e->Z * e->Z) ;
      e->X = e->X / el ;
      e->Y = e->Y / el ;
      e->Z = e->Z / el ;
}
