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

/*

                             裏側の面を見つける
                                 FILE=URA.c
                                 Create Date=1999.6.7

*/


#include <stdio.h>
#include <math.h>
#include "MODEL.h"

/*---------------------------------------------------------*/
void URA(int u, int w, P_MENN *LP, bekt *t, P_MENN *OP)
{
int i,j,k ;

for (j=0 ;j<u ;j++){
 for (i=0 ;i<w ;i++){
   for (k=0 ;k<LP[i].polyd ;k++){
        
      t[j].ps[i][k] = - ( OP[j].e.X * OP[j].P[k].X + OP[j].e.Y * OP[j].P[k].Y
                    + OP[j].e.Z * OP[j].P[k].Z - OP[j].e.X * LP[i].P[k].X
                    - OP[j].e.Y * LP[i].P[k].Y - OP[j].e.Z * LP[i].P[k].Z)
                    / ((OP[j].e.X) * (OP[j].e.X) + (OP[j].e.Y) * (OP[j].e.Y)
                    + (OP[j].e.Z) * (OP[j].e.Z)) ;
   }
 }
}
}
/*-------------------------------------------------------------*/


void URA_M(double ls,double ms,double ns,double *s,double wb)
{
double ex=0.0, ey, ez ;

ey=-sin((-wb)*rad) ;
ez=cos((-wb)*rad) ;

*s = (ex*ls+ey*ms+ez*ns)/(ex*ex+ey*ey+ez*ez) ;
}
